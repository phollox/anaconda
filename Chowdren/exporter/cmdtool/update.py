import urllib2
from cStringIO import StringIO
import zipfile

MASTER_ZIP = 'https://github.com/matpow2/anaconda/archive/master.zip'

print 'Downloading update.'
data = urllib2.urlopen(MASTER_ZIP).read()
print 'Done, extracting.'

fp = StringIO(data)
zipfile = zipfile.ZipFile(fp, 'r')
names = []
for name in zipfile.namelist():
    splitted = name.split('/')
    if len(splitted) >= 2 and (splitted[0] == 'anaconda-master' and
                               splitted[1] == 'tools'):
        continue
    names.append(name)

zipfile.extractall('.', names)
