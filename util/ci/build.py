import common as c
from config import qtc_version, os_name, plugin_name, pro_file, ts_files_dir
import os
import shutil
import sys
import multiprocessing

archive_extension = 'zip' if os_name.startswith('win') else 'tar.gz'
archive_name = '{}-{}-{}.{}'.format(plugin_name,
                                    qtc_version, os_name, archive_extension)

if len(sys.argv) > 1:  # handle subcommand
    if sys.argv[1] == 'artifact_name':
        c.print(archive_name)
    exit(0)

c.print('>> Building {} on {}'.format(plugin_name, os_name))

env_script = 'true'
path_separator = ':'
make_cmd = 'make -j{}'.format(multiprocessing.cpu_count())
qm_files_dir = 'share/qtcreator/translations'

if os_name == 'linux':
    plugin_bin = 'lib/qtcreator/plugins/lib{}.so'.format(plugin_name)
elif os_name == 'macos':
    plugin_bin = 'Qt Creator.app/Contents/PlugIns/lib{}.dylib'.format(
        plugin_name)
    qm_files_dir = 'Qt Creator.app/Contents/Resources/translations'
else:
    plugin_bin = 'lib/qtcreator/plugins/{}4.dll'.format(plugin_name)
    path_separator = ';'
    make_cmd = os.path.abspath('qtcreator/bin/jom')

    msvc_version = os.getenv('msvc_version', '2017/Community')
    env_script = 'C:/Program Files (x86)/Microsoft Visual Studio/{}/VC/Auxiliary/Build/'.format(
        msvc_version)
    if os_name == 'win32':
        env_script += 'vcvars32.bat'
    elif os_name == 'win64':
        env_script += 'vcvars64.bat'

os.environ['PATH'] = os.path.abspath(
    'qt/bin') + path_separator + os.environ['PATH']

qtc_dir = os.path.abspath('qtcreator')
os.environ['QTC_SOURCE'] = qtc_dir
os.environ['QTC_BUILD'] = qtc_dir

build_dir = 'build'
shutil.rmtree(build_dir, ignore_errors=True)
os.mkdir(build_dir)

env_cmd = '"{}"'.format(env_script)
qmake_cmd = 'qmake "{}"'.format(pro_file)

os.chdir(build_dir)
c.run('{} && {} && {}'.format(env_cmd, qmake_cmd, make_cmd))
os.chdir('..')

translation_cmd = 'lrelease "{}"'.format(pro_file)
c.run('{} && {}'.format(env_cmd, translation_cmd))

os.chdir('qtcreator')
translations = []
with os.scandir(ts_files_dir) as it:
    for entry in it:
        if not entry.name.endswith('.qm'):
            continue
        in_file = ts_files_dir + '/' + entry.name
        out_file = qm_files_dir + '/' + entry.name
        shutil.copy(in_file, out_file)
        translations.append(out_file)

c.archive(translations + [plugin_bin], '../' + archive_name)
