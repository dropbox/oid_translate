from distutils.core import setup, Extension

execfile("oid_translate/version.py")

_oid_translate = Extension("oid_translate._oid_translate",
                    libraries = ["netsnmp"],
                    sources = ["oid_translate/_oid_translate.c"])

kwargs = {
    "name": "oid_translate",
    "version": str(__version__),
    "packages": ["oid_translate"],
    "ext_modules" = [_oid_translate])
    "description": "Python OID/MIB Name Translator",
    # PyPi, despite not parsing markdown, will prefer the README.md to the
    # standard README. Explicitly read it here.
    "long_description": open("README").read(),
    "author": "Gary M. Josack",
    "maintainer": "Gary M. Josack",
    "author_email": "gary@dropbox.com",
    "maintainer_email": "gary@dropbox.com",
    "license": "MIT",
    "url": "https://github.com/dropbox/oid_translate",
    "download_url": "https://github.com/dropbox/oid_translate/archive/master.tar.gz",
    "classifiers": [
        "Programming Language :: Python",
        "Topic :: Software Development",
        "Topic :: Software Development :: Libraries",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ]
}

setup(**kwargs)
