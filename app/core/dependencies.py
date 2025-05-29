from fastapi import Depends, HTTPException, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
from app.core.authenticate import Authenticate

security = HTTPBearer()
auth = Authenticate()

def get_current_user_email(
    credentials: HTTPAuthorizationCredentials = Depends(security)
) -> str:
    token = credentials.credentials  # now this is safe
    try:
        payload = auth.decode_access_token(token)
        return payload.get("sub")
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail=str(e)
        )