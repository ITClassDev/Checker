from fastapi import FastAPI
import uvicorn
from pydantic import BaseModel
from checker import Checker
import os

app = FastAPI(title="ShTP Checker EPICBOX Service", version="0.0.1 EPICBOX")
PATH = "/home/stephan/Progs/ItClassDevelopment/Backend/app/static/users_data/uploads/tasks_source_codes"
checker = Checker()

class OneTask(BaseModel):
    source_code_path: str
    language: int
    tests: list
    env: dict
    submit_id: int

class Contest(BaseModel):
    git_url: str
    tests: list


@app.get("/")
async def index():
    return {"status": "Checker is ready"}

@app.post("/challenge")
async def check_challenge(payload: OneTask):
    return checker.check_one_task_thread(os.path.join(PATH, payload.source_code_path), payload.language, payload.tests, payload.env, payload.submit_id)

@app.post("/homework")
async def check_homework(payload: Contest):
    return checker.check_multiple_tasks(payload.git_url, payload.tests)

if __name__ == "__main__":
    uvicorn.run("service:app", port=7777, host="0.0.0.0", reload=False, headers=[("server", "Docker")])
