# app/main.py
from fastapi import FastAPI
from app.routers.youtube import router as youtube_router

app = FastAPI()

app.include_router(youtube_router, prefix="/youtube")