import common as c
from config import qtc_version, os_name
import os

c.print('>> Downloading Qt Creator {} for {}'.format(qtc_version, os_name))

if os_name == 'linux':
    os_url = 'linux_gcc_64_rhel72'
elif os_name == 'win32':
    os_url = 'windows_msvc2017_x86'
elif os_name == 'win64':
    os_url = 'windows_msvc2017_x64'
elif os_name == 'macos':
    os_url = 'mac_x64'

qtc_main_version = qtc_version[:qtc_version.rindex('.')]
base_url = 'https://download.qt.io/official_releases/qtcreator/{}/{}/installer_source/{}'\
    .format(qtc_main_version, qtc_version, os_url)

qtc_dir = 'qtcreator-{}'.format(qtc_version)
for archive in ['qtcreator.7z', 'qtcreator_dev.7z']:
    c.download(base_url + '/' + archive, archive)
    c.extract(archive, qtc_dir)

c.symlink(qtc_dir, 'qtcreator')

if os_name == 'macos':
    os.makedirs('qtcreator/bin', exist_ok=True)
    c.symlink(os.path.abspath('qtcreator/Qt Creator.app'),
              'qtcreator/bin/Qt Creator.app')
