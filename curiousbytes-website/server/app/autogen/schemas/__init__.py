from .youtube_schema import YoutubeSchema
from .resources_schema import ResourcesSchema
YoutubeSchema.model_rebuild(_types_namespace=globals())
ResourcesSchema.model_rebuild(_types_namespace=globals())
