from typing import List, Dict
from fastapi import APIRouter, Depends
from sqlalchemy.orm import Session
from app.db.session import get_db, SessionLocal
from app.db.crud import (
    get_all_videos,
    get_all_resources,
    get_all_shorts
)
from app.schemas.youtube import YouTubeVideoBase

router = APIRouter()

@router.get("/videos", response_model=List[YouTubeVideoBase])
def list_all_videos(db: Session = Depends(get_db)):
    return get_all_videos(db)

@router.get("/shorts", response_model=List[YouTubeVideoBase])
def list_all_videos(db: Session = Depends(get_db)):
    return get_all_shorts(db)

@router.get("/resources")
def list_all_resources(db: Session = Depends(get_db)):
    return get_all_resources(db)
