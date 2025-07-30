from .youtube_schema import YoutubeSchema
from .blogs_schema import BlogsSchema
from .resources_schema import ResourcesSchema
from .blog_videos_schema import BlogVideosSchema
from .resource_videos_schema import ResourceVideosSchema
from .tags_schema import TagsSchema
from .tag_links_schema import TagLinksSchema
YoutubeSchema.model_rebuild(_types_namespace=globals())
BlogsSchema.model_rebuild(_types_namespace=globals())
ResourcesSchema.model_rebuild(_types_namespace=globals())
BlogVideosSchema.model_rebuild(_types_namespace=globals())
ResourceVideosSchema.model_rebuild(_types_namespace=globals())
TagsSchema.model_rebuild(_types_namespace=globals())
TagLinksSchema.model_rebuild(_types_namespace=globals())
