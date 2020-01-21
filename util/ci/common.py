import os
import subprocess as sub
import urllib.request
from shutil import which
import zipfile
import tarfile
import functools


print = functools.partial(print, flush=True)


def run(cmd, capture_output=False, silent=False):
    print('>> Running', cmd)
    if capture_output:
        result = sub.run(cmd, check=True, shell=True, universal_newlines=True,
                         stdout=sub.PIPE, stderr=sub.STDOUT)
        if not silent:
            print(result.stdout)
    else:
        if not silent:
            result = sub.run(cmd, check=True, shell=True)
        else:
            result = sub.run(cmd, check=True, shell=True,
                             stdout=sub.DEVNULL, stderr=sub.DEVNULL)
    return result


def download(url, out, force=False):
    print('>> Downloading', url, 'as', out)
    if not force and os.path.exists(out):
        print('>>', out, 'already exists')
        return
    out_path = os.path.dirname(out)
    if len(out_path) > 0:
        os.makedirs(out_path, exist_ok=True)
    urllib.request.urlretrieve(url, out)


def extract(src, dest):
    abs_path = os.path.abspath(src)
    print('>> Extracting', abs_path, 'to', dest)
    if len(dest) > 0:
        os.makedirs(dest, exist_ok=True)

    if which('7z'):
        sub.run('7z x "{}" -o"{}"'.format(abs_path, dest),
                check=True, shell=True, input=b'S\n')
        return

    if which('cmake'):
        out = run('cmake -E tar t "{}"'.format(abs_path),
                  capture_output=True, silent=True)
        files = out.stdout.split('\n')
        already_exist = True
        for file in files:
            if not os.path.exists(os.path.join(dest, file)):
                already_exist = False
                break
        if already_exist:
            print('>> All files already exist')
            return
        sub.run('cmake -E tar xvf "{}"'.format(abs_path),
                check=True, shell=True, cwd=dest)
        return

    raise RuntimeError('No archiver to extract {} file'.format(src))


def archive(files, out):
    print('>> Archiving', files, 'into', out)
    if out.endswith('.zip'):
        arc = zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED)
        for f in files:
            arc.write(f)
        arc.close()
        return

    if out.endswith('.tar.gz'):
        arc = tarfile.open(out, 'w|gz')
        for f in files:
            arc.add(f)
        arc.close()
        return

    raise RuntimeError('No archiver to create {} file'.format(out))


def symlink(src, dest):
    print('>> Creating symlink', src, '=>', dest)
    norm_src = os.path.normcase(src)
    norm_dest = os.path.normcase(dest)
    if os.path.lexists(norm_dest):
        os.remove(norm_dest)
    os.symlink(norm_src, norm_dest,
               target_is_directory=os.path.isdir(norm_src))
