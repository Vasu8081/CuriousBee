from enum import Enum

class VideoTypes(str, Enum):
    Gate = 'GATE'
    ToppersTalk = 'TOPPERS_TALK'
    Interview = 'INTERVIEW'

class TagRefType(str, Enum):
    Video = 'VIDEO'
    Blog = 'BLOG'
    Resource = 'RESOURCE'

