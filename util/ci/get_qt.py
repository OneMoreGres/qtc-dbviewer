import common as c
from config import qt_modules, qt_version, os_name
import sys
import xml.etree.ElementTree as ET

c.print('>> Downloading Qt {} ({}) for {}'.format(
    qt_version, qt_modules, os_name))

qt_version_dotless = qt_version.replace('.', '')

if os_name == 'linux':
    os_url = 'linux_x64'
    kit_package = 'qt.qt5.{}.gcc_64'.format(qt_version_dotless)
    qt_dir_prefix = '{}/gcc_64'.format(qt_version)
elif os_name == 'win32':
    os_url = 'windows_x86'
    kit_package = 'qt.qt5.{}.win32_msvc2017'.format(qt_version_dotless)
    qt_dir_prefix = '{}/msvc2017'.format(qt_version)
elif os_name == 'win64':
    os_url = 'windows_x86'
    kit_package = 'qt.qt5.{}.win64_msvc2017_64'.format(qt_version_dotless)
    qt_dir_prefix = '{}/msvc2017_64'.format(qt_version)
elif os_name == 'macos':
    os_url = 'mac_x64'
    kit_package = 'qt.qt5.{}.clang_64'.format(qt_version_dotless)
    qt_dir_prefix = '{}/clang_64'.format(qt_version)

base_url = 'https://download.qt.io/online/qtsdkrepository/{}/desktop/qt5_{}' \
    .format(os_url, qt_version_dotless)
updates_file = 'Updates-{}-{}.xml'.format(qt_version, os_name)
c.download(base_url + '/Updates.xml', updates_file)

updates = ET.parse(updates_file)
updates_root = updates.getroot()
all_modules = {}
for i in updates_root.iter('PackageUpdate'):
    name = i.find('Name').text
    if name != kit_package:
        continue
    archives = i.find('DownloadableArchives').text.split(',')
    version = i.find('Version').text
    for archive in archives:
        archive = archive.strip()
        parts = archive.split('-')
        module_name = parts[0]
        all_modules[module_name] = {'package': name, 'file': version + archive}

if len(sys.argv) > 1:  # handle subcommand
    if sys.argv[1] == 'list':
        c.print('Available modules:')
        for k in iter(all_modules):
            c.print(k, '---', all_modules[k]['file'])
    exit(0)

for module in qt_modules:
    if module not in all_modules:
        continue
    file_name = all_modules[module]['file']
    package = all_modules[module]['package']
    c.download(base_url + '/' + package + '/' + file_name, file_name)
    c.extract(file_name, '.')

c.symlink(qt_dir_prefix, 'qt')

c.print('>> Updating license')
config_name = 'qt/mkspecs/qconfig.pri'
config = ''
with open(config_name, 'r') as f:
    config = f.read()

config = config.replace('Enterprise', 'OpenSource')
config = config.replace('licheck.exe', '')
config = config.replace('licheck64', '')
config = config.replace('licheck_mac', '')

with open(config_name, 'w') as f:
    f.write(config)
