"""Initial full schema

Revision ID: 76d3b0a39735
Revises: 3396b357dcaf
Create Date: 2025-06-14 18:11:55.548793

"""
from typing import Sequence, Union

from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql

# revision identifiers, used by Alembic.
revision: str = '76d3b0a39735'
down_revision: Union[str, None] = '3396b357dcaf'
branch_labels: Union[str, Sequence[str], None] = None
depends_on: Union[str, Sequence[str], None] = None


def upgrade() -> None:
    """Upgrade schema."""
    # ### commands auto generated by Alembic - please adjust! ###
    op.create_table('resources',
    sa.Column('resource_id', sa.UUID(), nullable=False),
    sa.Column('url', sa.String(), nullable=True),
    sa.PrimaryKeyConstraint('resource_id')
    )
    op.create_table('youtube',
    sa.Column('video_id', sa.String(), nullable=False),
    sa.Column('title', sa.String(), nullable=True),
    sa.Column('description', sa.TEXT(), nullable=True),
    sa.Column('published_at', sa.DateTime(), nullable=True),
    sa.Column('thumbnails', sa.JSON(), nullable=True),
    sa.Column('localized', sa.JSON(), nullable=True),
    sa.Column('channel_id', sa.String(), nullable=True),
    sa.Column('channel_title', sa.String(), nullable=True),
    sa.Column('raw_snippet', sa.JSON(), nullable=True),
    sa.Column('is_video', sa.Boolean(), nullable=True),
    sa.Column('video_type', sa.Enum('Gate', 'ToppersTalk', 'Interview', name='video_types'), nullable=False),
    sa.PrimaryKeyConstraint('video_id')
    )
    op.drop_index(op.f('ix_youtube_videos_video_id'), table_name='youtube_videos')
    op.drop_table('youtube_videos')
    # ### end Alembic commands ###


def downgrade() -> None:
    """Downgrade schema."""
    # ### commands auto generated by Alembic - please adjust! ###
    op.create_table('youtube_videos',
    sa.Column('video_id', sa.VARCHAR(), autoincrement=False, nullable=False),
    sa.Column('title', sa.VARCHAR(), autoincrement=False, nullable=True),
    sa.Column('description', sa.TEXT(), autoincrement=False, nullable=True),
    sa.Column('published_at', postgresql.TIMESTAMP(), autoincrement=False, nullable=True),
    sa.Column('thumbnails', postgresql.JSON(astext_type=sa.Text()), autoincrement=False, nullable=True),
    sa.Column('localized', postgresql.JSON(astext_type=sa.Text()), autoincrement=False, nullable=True),
    sa.Column('channel_id', sa.VARCHAR(), autoincrement=False, nullable=True),
    sa.Column('channel_title', sa.VARCHAR(), autoincrement=False, nullable=True),
    sa.Column('resources', postgresql.JSON(astext_type=sa.Text()), autoincrement=False, nullable=True),
    sa.Column('raw_snippet', postgresql.JSON(astext_type=sa.Text()), autoincrement=False, nullable=True),
    sa.Column('is_video', sa.BOOLEAN(), autoincrement=False, nullable=True),
    sa.Column('video_type', postgresql.ENUM('GateGeneral', 'GateStrategy', 'GateToppersInterview', name='videotype'), autoincrement=False, nullable=True),
    sa.PrimaryKeyConstraint('video_id', name=op.f('youtube_videos_pkey'))
    )
    op.create_index(op.f('ix_youtube_videos_video_id'), 'youtube_videos', ['video_id'], unique=False)
    op.drop_table('youtube')
    op.drop_table('resources')
    # ### end Alembic commands ###
