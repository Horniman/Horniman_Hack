# -*- coding: utf-8 -*-
"""
Setup Sphinx for documenting project.
"""
from __future__ import division, absolute_import, print_function, unicode_literals

import sys, os

os.environ['DJANGO_SETTINGS_MODULE'] = 'horni.settings'
sys.path.insert(0, os.path.abspath('..'))
sys.path.insert(0, os.path.abspath('../../lib/python2.7/site-packages/'))

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.todo',
    'sphinx.ext.viewcode',
]

master_doc = 'index'

project = u''
copyright = u'2013'
version = '2013'
release = '2013'

intersphinx_mapping = {
    'python':('http://docs.python.org/', None),
    'django':('https://docs.djangoproject.com/en/1.5/', 'https://docs.djangoproject.com/en/1.5/_objects/'),
}

todo_include_todos = True

autodoc_default_flags = ['members', 'undoc-members', 'private-members', 'show-inheritance']

