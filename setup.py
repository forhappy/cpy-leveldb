#!/usr/bin/python

# Copyright (c) 2011 Fu Haiping.
# See LICENSE for details.

import sys
from distutils.core import setup, Extension

extra_compile_args = ['-Wall', '-pedantic', '-I./leveldb/include/',
'-shared', '-std=gnu99', '-fPIC', '-g', '-D_GNU_SOURCE']
extra_link_args = ['-L/usr/local/lib/', '-static', '-lleveldb', '-lsnappy', '-lpthread']

setup(
	name = 'leveldb',
	version = '0.1',
	maintainer = 'Fu Haiping',
	maintainer_email = 'haipingf@gmail.com',
	url = 'http://code.google.com/p/cpy-leveldb/',

	classifiers = [
		'Development Status :: 4 - Beta',
		'Environment :: Other Environment',
		'Intended Audience :: Developers',
		'License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
		'Operating System :: POSIX',
		'Programming Language :: C',
		'Programming Language :: Python',
		'Programming Language :: Python :: 2.4',
		'Programming Language :: Python :: 2.5',
		'Programming Language :: Python :: 2.6',
		'Programming Language :: Python :: 2.7',
		'Topic :: Database',
		'Topic :: Software Development :: Libraries'
	],

	description = 'Python bindings for leveldb database library using c api',
	# long_description = 

	packages = ['leveldb'],
	package_dir = {'leveldb': ''},

	ext_modules = [
		Extension('leveldb',
			sources = [
				# python stuff
				'src/initmodule.c',
				'src/leveldb.c',
				'src/write_batch.c',
				'src/iterator.c',
				'src/snapshot.c',
                'src/range_iterator.c'
			],
			extra_compile_args = extra_compile_args,
			extra_link_args = extra_link_args
		)
	]
)
