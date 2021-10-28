import os.path as path
resources_archive_path = path.realpath(path.dirname(path.abspath(__file__)) + '/SMCE_Resources.zip')

__all__ = ['resources_archive_path']
