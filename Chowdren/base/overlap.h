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

#ifndef CHOWDREN_OVERLAP_H
#define CHOWDREN_OVERLAP_H

bool check_overlap(FrameObject * obj1, FrameObject * obj2);
bool check_overlap(ObjectList & list1, ObjectList & list2);
bool check_overlap(FrameObject * obj, ObjectList & list);
bool check_overlap(ObjectList & list, FrameObject * obj);
bool check_overlap(QualifierList & list1, ObjectList & list2);
bool check_overlap(ObjectList & list1, QualifierList & list2);
bool check_overlap(FrameObject * obj, QualifierList & list);
bool check_overlap(QualifierList & list, FrameObject * instance);
bool check_overlap(QualifierList & list1, QualifierList & list2);

bool check_overlap_save(FrameObject * obj1, FrameObject * obj2);
bool check_overlap_save(ObjectList & list1, ObjectList & list2);
bool check_overlap_save(FrameObject * obj, ObjectList & list);
bool check_overlap_save(ObjectList & list, FrameObject * obj);
bool check_overlap_save(QualifierList & list1, ObjectList & list2);
bool check_overlap_save(ObjectList & list1, QualifierList & list2);
bool check_overlap_save(FrameObject * obj, QualifierList & list);
bool check_overlap_save(QualifierList & list, FrameObject * instance);
bool check_overlap_save(QualifierList & list1, QualifierList & list2);

bool check_not_overlap(ObjectList & list1, ObjectList & list2);
bool check_not_overlap(QualifierList & list1, ObjectList & list2);
bool check_not_overlap(ObjectList & list1, QualifierList & list2);
bool check_not_overlap(QualifierList & list1, QualifierList & list2);
bool check_not_overlap(FrameObject * obj, ObjectList & list);
bool check_not_overlap(FrameObject * obj, QualifierList & list);

#endif // CHOWDREN_OVERLAP_H
