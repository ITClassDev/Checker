import epicbox
import os
import string
import random
import shutil


class Checker:
    def __init__(self):
        # Setup epicbox
        epicbox.configure(
            profiles={
                epicbox.Profile('python', 'stepik/epicbox-python:3.10.6'),
                epicbox.Profile('gcc', 'stepik/epicbox-gcc:10.2.1'),
            }
        )
        self.tasks_queue = []
        self.temp_workspace = "./workdir/"

    def name_gen(self, length=10):
        letters = string.ascii_lowercase
        result_str = ''.join(random.choice(letters) for i in range(length))
        return result_str

    def get_tasks_stat(self):
        tasks_active = 0
        task_pending = 0

    def check_one_task_thread(self, test_code_path, language, tests, env, submit_id) -> None:
        
        # FIXIT
        limits = {'cputime': env["cpu_time_limit"],
                  'memory': env["memory_limit"], 'realtime': env["real_time_limit"]}
        # language = {"py": 0, "cpp": 1}[source_file_name.split(".")[-1]]
        with open(test_code_path, "rb") as fd:
            if language == 0:  # python
                files = [{'name': 'main.py', 'content': fd.read()}]
                tests_statuses = []
                tests_passed = 0
                for test in tests:
                    result = epicbox.run(
                        'python', 'python3 main.py', files=files, limits=limits, stdin=test["input"])
                    test_status = result["stdout"].decode(
                        "utf-8").strip() == test["output"]
                    tests_passed += test_status
                    tests_statuses.append({"status": test_status, "error_info": result["stderr"].decode("utf-8"), "duration": result["duration"],
                                           "timeout": result["timeout"], "memoryout": result["oom_killed"]})
                # print("Checker finished")
                # callback((tests_passed == len(tests), tests_statuses, submit_id))
                return {"status": tests_passed == len(tests),
                        "tests": tests_statuses,
                        "submit_id": submit_id}
            elif language == 1:  # c++
                # like session storage
                with epicbox.working_directory() as workdir:
                    # compile
                    compile_res = epicbox.run('gcc', 'g++ -pipe -O2 -static -o main main.cpp',
                                              files=[
                                                  {'name': 'main.cpp', 'content': fd.read()}],
                                              workdir=workdir,
                                              limits={'cputime': 10, 'memory': 2048, 'realtime': 10})  # static build limits
                    if compile_res['exit_code'] == 0:
                        tests_statuses = []
                        tests_passed = 0
                        # iterate over tests
                        for test in tests:
                            result = epicbox.run('gcc', './main', stdin=test["input"],
                                                 limits=limits,
                                                 workdir=workdir)
                            test_status = result["stdout"].decode(
                                "utf-8").strip() == test["output"]
                            tests_passed += test_status
                            tests_statuses.append({"status": test_status, "error_info": result["stderr"].decode("utf-8"), "duration": result["duration"],
                                                   "timeout": result["timeout"], "memoryout": result["oom_killed"]})

                        return {
                            "status": tests_passed == len(tests),
                            "tests": tests_statuses,
                            "submit_id": submit_id
                        }
                    else:
                        return {
                            "status": False,
                            "tests": [],
                            "submit_id": submit_id
                        }

    def fetch_git(self, git_path):
        fetch_path = os.path.join(self.temp_workspace, self.name_gen())
        os.system(f"git clone {git_path} {fetch_path}")
        return fetch_path

    def check_multiple_tasks(self, git_path, tests):
        final_callback_data = {}
        source_path = self.fetch_git(git_path)
        with open(os.path.join(source_path, "header.h"), "rb") as header_fd:
            header_code = header_fd.read()
        with open(os.path.join(source_path, "funcs.cpp"), "rb") as funcs_fd:
            funcs_code = funcs_fd.read()

        for test_func in tests:
            base = '#include <iostream>\n#include <string.h>\n#include "header.h"\nint main(){\n'
            arg_counter = 0
            for arg in range(len(list(test_func.values())[0]["tests"][0]["input"].split("\n"))):
                arg_type = ["int", "string"][list(test_func.values())[
                    0]["types"][arg]]
                base += f"{arg_type} a{arg};\n    std::cin >> a{arg};\n    "
                arg_counter += 1

            main_code = base + f"std::cout << {list(test_func.keys())[0]}("
            for i in range(arg_counter-1):
                main_code += f"a{i},"
            main_code += f"a{arg_counter-1});\n"
            main_code += "}"
            # print(main_code)
            with epicbox.working_directory() as workdir:
                files = [{'name': 'main.cpp', 'content': main_code.encode(
                    "utf-8")}, {'name': 'header.h', 'content': header_code}, {'name': 'funcs.cpp', 'content': funcs_code}]
                compile_res = epicbox.run('gcc', 'g++ -pipe -O2 -static -o main main.cpp funcs.cpp',
                                          files=files,
                                          workdir=workdir,
                                          limits={'cputime': 10, 'memory': 2048, 'realtime': 10})  # static build limits FIXIT
                # print(compile_res["stderr"].decode("utf-8"))
                if compile_res['exit_code'] == 0:
                    tests_statuses = []
                    tests_passed = 0
                    test_set = list(test_func.values())[0]["tests"]
                    # iterate over tests
                    for test in test_set:
                        stdin_ = test["input"].encode("utf-8")
                        result = epicbox.run('gcc', './main', stdin=stdin_,
                                             # FIXIT NON STATIC
                                             limits={
                                                 'cputime': 2, 'memory': 200, 'realtime': 200},
                                             workdir=workdir)
                        test_status = result["stdout"].decode(
                            "utf-8").strip() == str(test["output"])
                        tests_passed += test_status
                        tests_statuses.append({"status": test_status, "error_info": result["stderr"], "duration": result["duration"],
                                               "timeout": result["timeout"], "memoryout": result["oom_killed"]})
                    final_callback_data[list(test_func.values())[0]["submit_id"]] = (
                        (tests_passed == len(test_set), tests_statuses))
                else:
                    return {"status": False}
                    
        #callback(final_callback_data, loop)  # send result data, to callback
        shutil.rmtree(source_path)
        return {"status": True, "tests": final_callback_data}

if __name__ == "__main__":
    pass