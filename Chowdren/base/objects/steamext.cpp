// Copyright (c) Mathias Kaerlev 2012-2015.
//
// This file is part of Anaconda.
//
// Anaconda is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Anaconda is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

#include "steamext.h"
#include <stdlib.h>
#include <iostream>
#include "fileio.h"
#include "path.h"
#include "platform.h"
#include "manager.h"
#include <ctype.h>

#ifdef CHOWDREN_IS_DESKTOP
#include <SDL.h>
#endif

// SteamGlobal

static std::string steam_language("English");

#ifdef CHOWDREN_ENABLE_STEAM
#include "steam/steam_api.h"
#include "steam/steamtypes.h"
#include "fileop.h"

class SteamGlobal
{
public:
    bool initialized;
    bool has_data;
    Frame::EventFunction download_success;
    Frame::EventFunction download_fail;

    SteamGlobal();
    static void on_close();
    bool is_ready();
    int init();

    STEAM_CALLBACK(SteamGlobal, receive_callback, UserStatsReceived_t,
                   receive_callback_data);

    void download_callback(RemoteStorageDownloadUGCResult_t * res);
};

static SteamGlobal global_steam_obj;

// export for platform_get_language

const std::string & get_steam_language()
{
    return steam_language;
}

#ifdef CHOWDREN_IS_FP
#include "objects/steamfp/frontend.cpp"
#endif

SteamGlobal::SteamGlobal()
: initialized(false), has_data(false),
  receive_callback_data(this, &SteamGlobal::receive_callback)
{
}

int init_steam()
{
    return global_steam_obj.init();
}

int SteamGlobal::init()
{
#if defined(CHOWDREN_FORCE_STEAM_OPEN) && defined(CHOWDREN_STEAM_APPID)
    if (SteamAPI_RestartAppIfNecessary(CHOWDREN_STEAM_APPID)) {
        return EXIT_FAILURE;
    }
#endif

    initialized = SteamAPI_Init();
    if (!initialized) {
        std::cout << "Could not initialize Steam API" << std::endl;
#ifdef CHOWDREN_FORCE_STEAM_OPEN
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Steam error",
                                 "Could not initialize Steam API. "
                                 "Please make sure you are logged in to Steam "
                                 "before opening the game.",
                                 NULL);
        return EXIT_FAILURE;
#endif
        return 0;
    }
	std::cout << "Initialized Steam API" << std::endl;

    bool debug_achievements = getenv("CHOWDREN_DEBUG_ACHIEVEMENTS") != NULL;
    if (debug_achievements) {
    	if (!SteamUserStats()->ResetAllStats(true))
    		std::cout << "Could not reset stats" << std::endl;
    }
	if (!SteamUserStats()->RequestCurrentStats())
		std::cout << "Could not request Steam stats" << std::endl;

#ifdef CHOWDREN_STEAM_APPID
    ISteamApps * ownapp = SteamApps();
    if (!ownapp->BIsSubscribedApp(CHOWDREN_STEAM_APPID)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Steam error",
                                 "Please purchase the Steam version of the "
                                 "game if you want to play it on Steam.",
                                 NULL);
        return EXIT_FAILURE;
    }
#endif
    steam_language = SteamApps()->GetCurrentGameLanguage();
    if (steam_language.empty())
        steam_language = "english";
    steam_language[0] = toupper(steam_language[0]);
    std::cout << "Detected Steam language: " << steam_language << std::endl;
    return 0;
}

void SteamGlobal::on_close()
{
    SteamAPI_Shutdown();
}

bool SteamGlobal::is_ready()
{
    return has_data;
}

void SteamGlobal::receive_callback(UserStatsReceived_t * callback)
{
    if (SteamUtils()->GetAppID() != callback->m_nGameID)
        return;
    has_data = true;
}

void SteamGlobal::download_callback(RemoteStorageDownloadUGCResult_t * res)
{
    if (res->m_eResult == k_EResultOK) {
        if (download_success == NULL)
            return;
        (manager.frame->*download_success)();
    } else {
        if (download_fail == NULL)
            return;
        (manager.frame->*download_fail)();
    }
}

#endif

#ifdef CHOWDREN_ENABLE_STEAM

struct SubCallback
{
    struct Call
    {
        CCallResult<SubCallback, SteamUGCRequestUGCDetailsResult_t> call;
    };

    UGCQueryHandle_t handle;
    Frame::EventFunction loop_callback, finish_callback;
    int received;
    vector<PublishedFileId_t> ids;
    vector<Call> calls;
    vector<SteamUGCDetails_t> details;
    bool subs;

    CCallResult<SubCallback, SteamUGCQueryCompleted_t> content_call;

    SubCallback()
    {
        subs = true;
    }

    void start(Frame::EventFunction loop, Frame::EventFunction finish)
    {
        loop_callback = loop;
        finish_callback = finish;

        if (subs)
            search_subs();
        else
            search_content();
    }

    void search_content()
    {
        handle = SteamUGC()->CreateQueryUserUGCRequest(
            SteamUser()->GetSteamID().GetAccountID(), k_EUserUGCList_Published,
            k_EUGCMatchingUGCType_Items,
            k_EUserUGCListSortOrder_CreationOrderDesc,
            0, CHOWDREN_STEAM_APPID, 1);
        SteamAPICall_t call_handle = SteamUGC()->SendQueryUGCRequest(handle);
        content_call.Set(call_handle, this, &SubCallback::on_content_callback);
    }

    void on_content_callback(SteamUGCQueryCompleted_t * result, bool fail)
    {
        if (fail || result->m_eResult != k_EResultOK) {
            (manager.frame->*finish_callback)();
            return;
        }

        received = 0;
        ids.resize(result->m_unNumResultsReturned);
        details.resize(result->m_unNumResultsReturned);

        for (int i = 0; i < int(result->m_unNumResultsReturned); ++i) {
            SteamUGC()->GetQueryUGCResult(handle, i, &details[i]);
            ids[i] = details[i].m_nPublishedFileId;
        }

       SteamUGC()->ReleaseQueryUGCRequest(handle);
       finish();
    }

    void search_subs()
    {
        int count = std::min<int>(50, SteamUGC()->GetNumSubscribedItems());
        if (count <= 0) {
            (manager.frame->*finish_callback)();
            return;
        }

        received = 0;
        ids.resize(count);
        calls.resize(count);
        details.resize(count);
        SteamUGC()->GetSubscribedItems(&ids[0], count);

        SteamAPICall_t call_handle;
        for (int i = 0; i < count; ++i) {
            std::cout << "Requesting for " << ids[i] << std::endl;
            call_handle = SteamUGC()->RequestUGCDetails(ids[i], 0);
            calls[i].call.Set(call_handle, this, &SubCallback::on_callback);
        }
    }

    void on_callback(SteamUGCRequestUGCDetailsResult_t * result, bool fail)
    {
        std::cout << "Callback received!" << std::endl;
        if (fail) {
            std::cout << "Failed callback" << std::endl;
            return;
        }
        SteamUGCDetails_t & d = result->m_details;
        int index;
        for (index = 0; index < int(ids.size()); ++index) {
            if (ids[index] == d.m_nPublishedFileId)
                break;
        }

        details[index] = d;
        received++;
        if (received < int(ids.size()))
            return;

        finish();
    }

    void finish()
    {
        SteamObject::SubResult & r = SteamObject::sub_result;
        for (int i = 0; i < int(ids.size()); ++i) {
            SteamUGCDetails_t & d = details[i];
            r.index = i;
            r.cloud_path = d.m_pchFileName;
            r.title = d.m_rgchTitle;
            r.publish_id = i;
            (manager.frame->*loop_callback)();
        }

        (manager.frame->*finish_callback)();
    }
};

inline std::string trim_spaces(const std::string & value)
{
    int i;
    for (i = 0; i < int(value.size()); ++i) {
        if (value[i] != ' ')
            break;
    }
    int ii;
    for (ii = int(value.size())-1; ii >= 0; --ii) {
        if (value[ii] != ' ')
            break;
    }
    return value.substr(i, (ii-i) + 1);
}

struct UploadCallback
{
    bool has_id;
    PublishedFileId_t current_id;
    std::string file;
    std::string cloud_file;
    std::string preview;
    std::string preview_cloud_file;
    unsigned int appid;
    std::string title;
    std::string description;
    int visibility;

    enum {
        SET_TITLE = 1 << 0,
        SET_FILE = 1 << 1,
        SET_PREVIEW = 1 << 2,
        SET_DESCRIPTION = 1 << 3,
        SET_VISIBILITY = 1 << 4,
        SET_TAGS = 1 << 5
    };

    unsigned int flags;

    bool uploading;

    CCallResult<UploadCallback,
                RemoteStorageUpdatePublishedFileResult_t> update_call;
    CCallResult<UploadCallback, RemoteStoragePublishFileResult_t> new_call;
    CCallResult<UploadCallback, RemoteStorageFileShareResult_t> share_call;
    CCallResult<UploadCallback, RemoteStorageFileShareResult_t> preview_call;

    Frame::EventFunction done_callback, fail_callback;

    std::string error;

    vector<std::string> tags;
    vector<const char*> tags_c;

    SteamParamStringArray_t steam_tags;
    int run_callbacks;

    UploadCallback()
    {
        has_id = false;
        appid = CHOWDREN_STEAM_APPID;
        visibility = 0;
        uploading = false;

        done_callback = NULL;
        fail_callback = NULL;

        flags = 0;
        run_callbacks = 0;
    }

    void set_callbacks(Frame::EventFunction done, Frame::EventFunction fail)
    {
        done_callback = done;
        fail_callback = fail;
    }

    void set_title(const std::string & value)
    {
        flags |= SET_TITLE;
        title = value;
    }

    void set_file(const std::string & value)
    {
        flags |= SET_FILE;
        file = value;
        cloud_file = get_path_filename(file);
    }

    void set_preview(const std::string & value)
    {
        if (value.empty())
            return;
        flags |= SET_PREVIEW;
        preview = value;
        preview_cloud_file = "preview" + number_to_string((uint64_t)current_id)
                             + "." + get_path_ext(preview);
    }

    void set_visibility(int value)
    {
        flags |= SET_VISIBILITY;
        visibility = value;
    }

    void set_tags(const std::string & value)
    {
        flags |= SET_TAGS;
        tags.clear();

        split_string(value, ',', tags);
        tags_c.resize(tags.size());
        int i = 0;
        vector<std::string>::iterator it;
        for (it = tags.begin(); it != tags.end(); ++it) {
            *it = trim_spaces(*it);
            tags_c[i] = (*it).c_str();
            std::cout << "Tag: '" << (*it) << "'" << std::endl;
            i++;
        }

        steam_tags.m_ppStrings = &tags_c[0];
        steam_tags.m_nNumStrings = tags_c.size();
    }

    void set_description(const std::string & value)
    {
        flags |= SET_DESCRIPTION;
        description = value;
    }

    void share_callback(RemoteStorageFileShareResult_t * result, bool fail)
    {
        if (result && (result->m_eResult != k_EResultOK || fail)) {
            error = "Error: " + number_to_string(result->m_eResult);
            call_fail();
            return;
        }

        if (!(flags & SET_PREVIEW)) {
            preview_callback(NULL, false);
            return;
        }

        std::string data;
        if (!read_file(preview.c_str(), data)) {
            error = "File not found";
            call_fail();
            return;
        }
        SteamRemoteStorage()->FileWrite(preview_cloud_file.c_str(),
                                        &data[0], data.size());
        SteamAPICall_t call_handle;
        call_handle = SteamRemoteStorage()->FileShare(
            preview_cloud_file.c_str());
        preview_call.Set(call_handle, this, &UploadCallback::preview_callback);
    }

    void preview_callback(RemoteStorageFileShareResult_t * result, bool fail)
    {
        if (result && (result->m_eResult != k_EResultOK || fail)) {
            error = "Error: " + number_to_string(result->m_eResult);
            call_fail();
            return;
        }

        if (has_id) {
            do_update();
            return;
        }

        ERemoteStoragePublishedFileVisibility vis =
            (ERemoteStoragePublishedFileVisibility)visibility;
        SteamAPICall_t call_handle;
        call_handle = SteamRemoteStorage()->PublishWorkshopFile(
            cloud_file.c_str(), preview_cloud_file.c_str(), appid,
            title.c_str(), description.c_str(), vis, &steam_tags,
            k_EWorkshopFileTypeCommunity);
        new_call.Set(call_handle, this, &UploadCallback::new_callback);
    }

    void new_callback(RemoteStoragePublishFileResult_t * result, bool fail)
    {
        if (result->m_eResult != k_EResultOK || fail) {
            error = "Error: " + number_to_string(result->m_eResult);
            call_fail();
            return;
        }

        call_done();
    }

    void call_fail()
    {
        uploading = false;
        flags = 0;
        run_callbacks = 2;
    }

    void call_done()
    {
        uploading = false;
        flags = 0;
        run_callbacks = 1;
    }

    void check_callbacks()
    {
        if (run_callbacks == 0)
            return;
        if (run_callbacks == 1 && done_callback)
            (manager.frame->*done_callback)();
        else if (run_callbacks == 2 && fail_callback)
            (manager.frame->*fail_callback)();
        run_callbacks = 0;
    }

    void on_create_callback(CreateItemResult_t * result, bool fail)
    {
        if (result->m_eResult != k_EResultOK || fail) {
            uploading = false;
            error = "Error: " + number_to_string(result->m_eResult);
            call_fail();
            return;
        }

        has_id = true;
        current_id = result->m_nPublishedFileId;
        start();
    }

    void start()
    {
        uploading = true;

        if (!(flags & SET_FILE)) {
            share_callback(NULL, false);
            return;
        }

        std::string data;
        if (!read_file(file.c_str(), data)) {
            error = "File not found";
            call_fail();
            return;
        }
        SteamRemoteStorage()->FileWrite(cloud_file.c_str(),
                                        &data[0], data.size());
        SteamAPICall_t call_handle;
        call_handle = SteamRemoteStorage()->FileShare(cloud_file.c_str());
        share_call.Set(call_handle, this, &UploadCallback::share_callback);
    }

    void do_update()
    {
        PublishedFileUpdateHandle_t handle;
        handle = SteamRemoteStorage()->CreatePublishedFileUpdateRequest(
            current_id);
        if (flags & SET_TITLE) {
            if (!SteamRemoteStorage()->UpdatePublishedFileTitle(
                    handle, title.c_str()))
                std::cout << "Could not set item title" << std::endl;
        }
        if (flags & SET_DESCRIPTION) {
            if (!SteamRemoteStorage()->UpdatePublishedFileDescription(
                    handle, description.c_str()))
                std::cout << "Could not set item description" << std::endl;
        }

        if (flags & SET_PREVIEW) {
            if (!SteamRemoteStorage()->UpdatePublishedFilePreviewFile(
                    handle, preview_cloud_file.c_str()))
                std::cout << "Could not set item preview" << std::endl;
        }

        if (flags & SET_FILE) {
            if (!SteamRemoteStorage()->UpdatePublishedFileFile(
                    handle, cloud_file.c_str()))
                std::cout << "Could not set item content" << std::endl;
        }

        if (flags & SET_TAGS) {
            if (!SteamRemoteStorage()->UpdatePublishedFileTags(
                    handle, &steam_tags))
                std::cout << "Could not set item tags" << std::endl;
        }

        if (flags & SET_VISIBILITY) {
            ERemoteStoragePublishedFileVisibility vis =
                (ERemoteStoragePublishedFileVisibility)visibility;
            if (!SteamRemoteStorage()->UpdatePublishedFileVisibility(
                    handle, vis))
                std::cout << "Could not set item visibility" << std::endl;
        }

        flags = 0;
        uploading = true;
        SteamAPICall_t call_handle;
        call_handle = SteamRemoteStorage()->CommitPublishedFileUpdate(handle);
        update_call.Set(call_handle, this,
                        &UploadCallback::on_update_callback);
    }

    void on_update_callback(RemoteStorageUpdatePublishedFileResult_t * result,
                            bool fail)
    {
        if (result->m_eResult != k_EResultOK || fail) {
            error = "Error: " + number_to_string(result->m_eResult);
            call_fail();
            return;
        }

        call_done();
    }
};

static SubCallback ugc_list_callback;
static UploadCallback ugc_upload;
#endif

// SteamObject

SteamObject::SteamObject(int x, int y, int type_id)
: FrameObject(x, y, type_id)
{
}

bool SteamObject::is_ready()
{
#ifdef CHOWDREN_ENABLE_STEAM
    return global_steam_obj.is_ready();
#else
    return true;
#endif
}

void SteamObject::update()
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return;
    SteamAPI_RunCallbacks();
    ugc_upload.check_callbacks();
#endif
}

int SteamObject::get_int(const std::string & name)
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return 0;
    int32 ret;
    if (!SteamUserStats()->GetStat(name.c_str(), &ret))
        return 0;
    return ret;
#else
    return 0;
#endif
}

void SteamObject::set_int(const std::string & name, int value)
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return;
    int32 v = value;
    SteamUserStats()->SetStat(name.c_str(), v);
#endif
}

void SteamObject::unlock_achievement(const std::string & name)
{
#ifndef NDEBUG
    std::cout << "Unlock achievement: " << name << std::endl;
#endif

#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return;
    SteamUserStats()->SetAchievement(name.c_str());
    SteamUserStats()->StoreStats();
#endif

#ifndef CHOWDREN_IS_DESKTOP
    platform_unlock_achievement(name);
#endif
}

void SteamObject::request_user_data()
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return;
    SteamUserStats()->RequestCurrentStats();
#endif
}

void SteamObject::store_user_data()
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return;
    SteamUserStats()->StoreStats();
#endif
}

void SteamObject::set_search(bool subs)
{
#ifdef CHOWDREN_ENABLE_STEAM
    ugc_list_callback.subs = subs;
#endif
}

bool SteamObject::get_content(Frame::EventFunction loop,
                              Frame::EventFunction finish)
{
#ifdef CHOWDREN_ENABLE_STEAM
    ugc_list_callback.start(loop, finish);
    return true;
#else
    return false;
#endif
}

void SteamObject::clear_achievement(const std::string & name)
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return;
    SteamUserStats()->ClearAchievement(name.c_str());
    SteamUserStats()->StoreStats();
#endif
}

void SteamObject::clear_achievements()
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return;
    SteamUserStats()->ResetAllStats(true);
    SteamUserStats()->StoreStats();
#endif
}

bool SteamObject::is_achievement_unlocked(const std::string & name)
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return false;
    bool achieved;
    SteamUserStats()->GetAchievement(name.c_str(), &achieved);
    return achieved;
#else
    return false;
#endif
}

int SteamObject::get_unlocked(const std::string & name)
{
    return (int)is_achievement_unlocked(name);
}

void SteamObject::upload(const std::string & name)
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return;
    std::string filename = get_path_filename(name);
    const char * filename_c = filename.c_str();
    char * data;
    size_t size;
    if (!read_file(filename_c, &data, &size))
        return;
    SteamRemoteStorage()->FileWrite(filename_c, data, size);
#endif
}

void SteamObject::download(const std::string & name)
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return;
    std::string filename = get_path_filename(name);
    const char * filename_c = filename.c_str();
    if (!SteamRemoteStorage()->FileExists(filename_c))
        return;

    int32 size = SteamRemoteStorage()->GetFileSize(filename_c);
    std::string value;
    value.resize(size);
    SteamRemoteStorage()->FileRead(filename_c, &value[0], size);
    FSFile fp(name.c_str(), "w");
    if (!fp.is_open())
        return;
    fp.write(&value[0], value.size());
    fp.close();
#endif
}

bool SteamObject::has_app(int app)
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return false;
    return SteamApps()->BIsSubscribedApp(app);
#else
    return false;
#endif
}

#ifdef CHOWDREN_ENABLE_STEAM
struct DownloadCall
{
    CCallResult<DownloadCall, RemoteStorageDownloadUGCResult_t> call;

	void on_callback(RemoteStorageDownloadUGCResult_t * result, bool fail)
    {
        global_steam_obj.download_callback(result);
        delete this;
    }
};

#endif

void SteamObject::download(const std::string & name, int priority,
                           int content_id,
                           Frame::EventFunction success,
                           Frame::EventFunction fail)
{
#ifdef CHOWDREN_ENABLE_STEAM
    global_steam_obj.download_success = success;
    global_steam_obj.download_fail = fail;
    std::string filename = convert_path(name);
    SteamAPICall_t handle;
    UGCHandle_t id = ugc_list_callback.details[content_id].m_hFile;
    handle = SteamRemoteStorage()->UGCDownloadToLocation(id, filename.c_str(),
                                                         priority);
    DownloadCall * call = new DownloadCall();
	call->call.Set(handle, call, &DownloadCall::on_callback);
    std::cout << "Download: " << id << " " << name << " " << priority
        << std::endl;
#endif
}

const std::string & SteamObject::get_user_name()
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return empty_string;
    static std::string name;
    name = SteamFriends()->GetPersonaName();
    return name;
#else
    return empty_string;
#endif
}

int SteamObject::get_user_id()
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return 0;
    return SteamUser()->GetSteamID().GetAccountID();
#else
    return 0;
#endif
}

bool SteamObject::is_activated()
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
#ifdef CHOWDREN_FORCE_STEAM_OPEN
        return false;
#else
        return true;
#endif
    SteamUserStats()->RequestCurrentStats();
    ISteamApps * ownapp = SteamApps();
    return ownapp->BIsSubscribedApp(CHOWDREN_STEAM_APPID);
#else
	return true;
#endif
}

bool SteamObject::is_active(const std::string & session_id)
{
#ifdef CHOWDREN_ENABLE_STEAM
    return ugc_upload.uploading;
#else
    return false;
#endif
}

bool SteamObject::is_connected()
{
#ifdef CHOWDREN_ENABLE_STEAM
    if (!global_steam_obj.initialized)
        return false;
    return true;
#else
    return false;
#endif
}

void SteamObject::reset_uncommited_changes()
{
    std::cout << "reset_uncommited_changes not implemented" << std::endl;
}

void SteamObject::reset_file_changes()
{
    std::cout << "reset_file_changes not implemented" << std::endl;
}

void SteamObject::reset_files()
{
    std::cout << "clear_files not implementd" << std::endl;
}

void SteamObject::reset_changes()
{
    std::cout << "reset_changes not implementd" << std::endl;
}

void SteamObject::set_preview_latest(const std::string & local_path,
                                     const std::string & cloud_path,
                                     bool overwrite)
{
#ifdef CHOWDREN_ENABLE_STEAM
    std::cout << "Set preview: " << local_path << std::endl;
    ugc_upload.set_preview(local_path);
#endif
}

void SteamObject::upload_changes(Frame::EventFunction done,
                                 Frame::EventFunction fail)
{
#ifdef CHOWDREN_ENABLE_STEAM
    ugc_upload.set_callbacks(done, fail);
    ugc_upload.start();
#endif
}

const std::string & SteamObject::get_error()
{
#ifdef CHOWDREN_ENABLE_STEAM
    return ugc_upload.error;
#else
    return empty_string;
#endif
}

void SteamObject::set_tags(const std::string & tags)
{
#ifdef CHOWDREN_ENABLE_STEAM
    ugc_upload.set_tags(tags);
    std::cout << "Set tags: " << tags << std::endl;
#endif
}

void SteamObject::set_description(const std::string & value)
{
#ifdef CHOWDREN_ENABLE_STEAM
    ugc_upload.set_description(value);
    std::cout << "Set description: " << value << std::endl;
#endif
}

void SteamObject::set_file(const std::string & local_path,
                           const std::string & cloud_path,
                           bool overwrite)
{
#ifdef CHOWDREN_ENABLE_STEAM
    ugc_upload.set_file(local_path);
    std::cout << "Set file: " << local_path << std::endl;
#endif
}

void SteamObject::set_content_title(const std::string & title)
{
#ifdef CHOWDREN_ENABLE_STEAM
    ugc_upload.set_title(title);
    std::cout << "Set content title: " << title << std::endl;
#endif
}

void SteamObject::set_content_appid(unsigned int value)
{
#ifdef CHOWDREN_ENABLE_STEAM
    ugc_upload.appid = value;
#endif
}

void SteamObject::set_content_visibility(int value)
{
#ifdef CHOWDREN_ENABLE_STEAM
    ugc_upload.set_visibility(value);
    std::cout << "Set visibility: " << value << std::endl;
#endif
}

void SteamObject::start_content_change(unsigned int content_id,
                                       const std::string & session_id)
{
#ifdef CHOWDREN_ENABLE_STEAM
    PublishedFileId_t id;
    id = ugc_list_callback.details[content_id].m_nPublishedFileId;
    ugc_upload.has_id = true;
    ugc_upload.current_id = id;
#endif
}

void SteamObject::start_publish(const std::string & session_id)
{
#ifdef CHOWDREN_ENABLE_STEAM
    ugc_upload.has_id = false;
#endif
}

bool SteamObject::is_enabled()
{
#ifdef CHOWDREN_ENABLE_STEAM
    return true;
#else
    return false;
#endif
}

#if !defined(CHOWDREN_ENABLE_STEAM) && defined(CHOWDREN_IS_FP)
void SteamObject::find_board(int char_id, int stage_id)
{
}

void SteamObject::upload_crystal(int value)
{
}

void SteamObject::upload_time(int value)
{
}
#endif

SteamObject::SubResult SteamObject::sub_result;
