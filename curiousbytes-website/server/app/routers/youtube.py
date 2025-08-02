from app.autogen.models import *
from app.autogen.schemas import *
from app.autogen.models.enums import *
from app.db.session import get_db
from fastapi import APIRouter, Depends
from sqlalchemy.orm import Session

router = APIRouter()

@router.get("/youtube/gate/videos", response_model=list[YoutubeSchema])
def get_all_gate_videos(db: Session = Depends(get_db)):
    videos = db.query(Youtube).filter(Youtube.__table__.c.video_type == VideoTypes.Gate).all()
    return [video.to_schema() for video in videos]

@router.get("/youtube/interviews/videos", response_model=list[YoutubeSchema])
def get_all_interview_videos(db: Session = Depends(get_db)):
    videos = db.query(Youtube).filter(Youtube.__table__.c.video_type == VideoTypes.Interview).all()
    return [video.to_schema() for video in videos]

@router.get("/resources", response_model=list[ResourcesSchema])
def get_all_resources(db: Session = Depends(get_db)):
    resources = db.query(Resources).all()
    return [resource.to_schema() for resource in resources]

@router.get("/blogs", response_model=list[BlogsSchema])
def get_all_blogs(db: Session = Depends(get_db)):
    blogs = db.query(Blogs).all()
    return [blog.to_schema() for blog in blogs]

@router.get("/blogs/{slug}", response_model=BlogsSchema)
def get_blog_by_slug(slug: str, db: Session = Depends(get_db)):
    blog = db.query(Blogs).filter(Blogs.slug == slug).first()
    if not blog:
        raise HTTPException(status_code=404, detail="Blog not found")
    return blog.to_schema()