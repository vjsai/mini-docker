from setuptools import setup, Extension

sys_module = Extension('system', sources=['system.c'])

setup(name='system',
      version='1.0',
      description='',
      ext_modules=[sys_module])
