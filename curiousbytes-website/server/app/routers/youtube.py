from app.autogen.models import *
from app.autogen.schemas import *
from app.autogen.models.enums import *
from app.db.session import get_db
from fastapi import APIRouter, Depends
from sqlalchemy.orm import Session

router = APIRouter()

@router.get("/gate/videos", response_model=list[YoutubeSchema])
def get_all_gate_videos(db: Session = Depends(get_db)):
    videos = db.query(Youtube).filter(Youtube.__table__.c.video_type == VideoTypes.Gate).all()
    return [video.to_schema() for video in videos]

@router.get("/interviews/videos", response_model=list[YoutubeSchema])
def get_all_interview_videos(db: Session = Depends(get_db)):
    videos = db.query(Youtube).filter(Youtube.__table__.c.video_type == VideoTypes.Interview).all()
    return [video.to_schema() for video in videos]

@router.get("/resources", response_model=list[ResourcesSchema])
def get_all_resources(db: Session = Depends(get_db)):
    resources = db.query(Resources).all()
    return [resource.to_schema() for resource in resources]