# app/main.py
from fastapi import FastAPI
from app.routers.youtube import router as youtube_router
from prometheus_fastapi_instrumentator import Instrumentator

app = FastAPI()
Instrumentator().instrument(app).expose(app)

app.include_router(youtube_router, prefix="/api/youtube")