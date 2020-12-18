import common as c
from config import qtc_version, os_name
import os
import shutil
import glob

c.print('>> Downloading Qt Creator {} for {}'.format(qtc_version, os_name))

if os_name == 'linux':
    os_url = 'linux_x64'
elif os_name == 'win32':
    os_url = 'windows_x86'
elif os_name == 'win64':
    os_url = 'windows_x64'
elif os_name == 'macos':
    os_url = 'mac_x64'

src_base = 'qt-creator-opensource-src-' + qtc_version
qtc_dir = src_base
qtc_main_version = qtc_version[:qtc_version.rindex('.')]
base_url = 'https://download.qt.io/official_releases/qtcreator/{}/{}'\
    .format(qtc_main_version, qtc_version)

src_ext = 'zip' if os_name.startswith('win') else 'tar.xz'
src_archive = src_base + '.' + src_ext
c.download(base_url + '/' + src_archive, src_archive)
c.extract(src_archive, '.')

if os_name == 'macos':
    c.print('>> Workaround')
    shutil.rmtree(
        qtc_dir + '/src/shared/qbs/examples/cocoa-application/CocoaApplication/en_US.lproj', ignore_errors=True)

for archive in ['qtcreator.7z', 'qtcreator_dev.7z']:
    url = base_url + '/installer_source/{}/{}'.format(os_url, archive)
    c.download(url, archive)
    c.extract(archive, qtc_dir)

if os_name.startswith('win'):
    c.print('>> Workaround (renaming .libs)')
    for name in glob.glob('{}/lib/qtcreator/**/*.lib'.format(qtc_dir), recursive=True):
        if name.endswith('4.lib'):
            continue
        newname = name[:-4]+'4.lib'
        os.rename(name, newname)
        c.print('{} -> {}'.format(name, newname))

c.symlink(qtc_dir, 'qtcreator')

if os_name == 'macos':
    os.makedirs('qtcreator/bin', exist_ok=True)
    c.symlink(os.path.abspath('qtcreator/Qt Creator.app'),
              'qtcreator/bin/Qt Creator.app')
