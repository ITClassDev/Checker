import requests
import json

payload = {
    "language": "cpp",
    "github_link": "https://github.com/ret7020/CppHeaders.git",
    "test_type": "header_test",
    "tests_description": [
        {"name": "itc_abs", "tests": [
            {"input": "-34", "output": "34", "demo": True},
            {"input": "10009", "output": "10009"},
            {"input": "0", "output": "0"},
            {"input": "-2147483647", "output": "2147483647"}   ],
        "submit_id": "2cd1ad27-f425-402b-865b-9f784fc78c11", "types": {"in": ["int"], "out": "int"},
        "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_fabs", "tests": [
        #     {"input": "-34.78", "output": "34.00000"},
        #     {"input": "34.99", "output": "34.00000"},
        #     {"input": "-3333444234.99", "output": "2147483647.00000"},
        #     {"input": "0.0", "output": "0.00000"},
        #     {"input": "-2147483648234324324324456546.345345435345423899", "output": "-2147483647.00000"}   ],
        # "submit_id": "11", "types": {"in": ["double"], "out": "double"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_revnbr", "tests": [
        #     {"input": "100", "output": "1"},
        #     {"input": "205", "output": "502"},
        #     {"input": "999", "output": "999"},
        #     {"input": "810", "output": "18"},
        #     {"input": "981", "output": "189"},
        #     {"input": "-123", "output": "-321"},
        #     {"input": "-200", "output": "-2"}   ],
        # "submit_id": "12", "types": {"in": ["int"], "out": "int"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_iseven", "tests": [
        #     {"input": "100", "output": "1"},
        #     {"input": "205", "output": "0"},
        #     {"input": "2147483647", "output": "0"},
        #     {"input": "-2147483648", "output": "1"},   
        #     {"input": "0", "output": "1"}   ],
        # "submit_id": "13", "types": {"in": ["int"], "out": "bool"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_max", "tests": [
        #     {"input": "100\n100", "output": "100"},
        #     {"input": "-23\n2147483647", "output": "2147483647"},
        #     {"input": "-2147483648\n2147483647", "output": "2147483647"},
        #     {"input": "0\n0", "output": "0"},
        #     {"input": "89\n2", "output": "89"}   ],
        # "submit_id": "14", "types": {"in": ["int", "int"], "out": "int"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_min", "tests": [
        #     {"input": "100\n100", "output": "100"},
        #     {"input": "-23\n2147483647", "output": "-23"},
        #     {"input": "-2147483648\n2147483647", "output": "-2147483648"},
        #     {"input": "0\n0", "output": "0"},
        #     {"input": "89\n2", "output": "2"}   ],
        # "submit_id": "15", "types": {"in": ["int", "int"], "out": "int"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },
        
        # {"name": "itc_fmax", "tests": [
        #     {"input": "100\n100", "output": "100.00000"},
        #     {"input": "-23\n2147483647", "output": "2147483647.00000"},
        #     {"input": "-2147483648\n2147483647", "output": "2147483647.00000"},
        #     {"input": "0\n0", "output": "0.00000"},
        #     {"input": "89.34\n2", "output": "89.34000"}   ],
        # "submit_id": "16", "types": {"in": ["double", "double"], "out": "double"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_fmin", "tests": [
        #     {"input": "100\n100", "output": "100.00000"},
        #     {"input": "-23.01\n2147483647.0", "output": "-23.01000"},
        #     {"input": "-2147483648\n2147483647", "output": "-2147483648.00000"},
        #     {"input": "0\n0", "output": "0.00000"},
        #     {"input": "89\n2", "output": "2.00000"}   ],
        # "submit_id": "17", "types": {"in": ["double", "double"], "out": "double"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_sqrt", "tests": [
        #     {"input": "100", "output": "10"},
        #     {"input": "16", "output": "4"},
        #     {"input": "256", "output": "16"},
        #     {"input": "35", "output": "-1"},
        #     {"input": "2147483647", "output": "-1"},
        #     {"input": "-2147483648", "output": "-1"},
        #     {"input": "0", "output": "0"}   ],
        # "submit_id": "18", "types": {"in": ["int"], "out": "int"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },
        
        # {"name": "itc_skv", "tests": [
        #     {"input": "10", "output": "100"},
        #     {"input": "1", "output": "1"},
        #     {"input": "3", "output": "9"},
        #     {"input": "-3", "output": "-1"},
        #     {"input": "0", "output": "-1"}   ],
        # "submit_id": "19", "types": {"in": ["int"], "out": "int"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },
        
        # {"name": "itc_spr", "tests": [
        #     {"input": "2\n3", "output": "6"},
        #     {"input": "3\n3", "output": "9"},
        #     {"input": "2\n-3", "output": "-1"},
        #     {"input": "-2\n3", "output": "-1"},
        #     {"input": "0\n3", "output": "-1"},
        #     {"input": "-2\n-3", "output": "-1"}   ],
        # "submit_id": "20", "types": {"in": ["int", "int"], "out": "int"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },
        
        # {"name": "itc_str", "tests": [
        #     {"input": "3\n4\n5", "output": "6"},
        #     {"input": "2\n3\n4", "output": "-1"},
        #     {"input": "4\n5\n9", "output": "-1"},
        #     {"input": "9\n5\n4", "output": "-1"},
        #     {"input": "10\n5\n1", "output": "-1"},
        #     {"input": "-10\n5\n1", "output": "-1"},
        #     {"input": "10\n-5\n1", "output": "-1"},
        #     {"input": "10\n5\n-1", "output": "-1"},
        #     {"input": "4\n5\n9", "output": "-1"}   ],
        # "submit_id": "21", "types": {"in": ["int", "int", "int"], "out": "int"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_scir", "tests": [
        #     {"input": "12", "output": "452.16000"},
        #     {"input": "2", "output": "12.56000"},
        #     {"input": "-2", "output": "-1.00000"}   ],
        # "submit_id": "22", "types": {"in": ["int"], "out": "double"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_pow", "tests": [
        #     {"input": "2\n3", "output": "8.00000"},
        #     {"input": "2\n5", "output": "32.00000"},
        #     {"input": "-2\n3", "output": "-8.00000"},
        #     {"input": "2\n-3", "output": "0.12500"},
        #     {"input": "-2\n0", "output": "1.00000"},
        #     {"input": "0\n5", "output": "0.00000"},
        #     {"input": "2\n0", "output": "1.00000"}   ],
        # "submit_id": "23", "types": {"in": ["int", "int"], "out": "double"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_ispositive", "tests": [
        #     {"input": "123", "output": "1"},
        #     {"input": "-1123", "output": "0"},
        #     {"input": "0", "output": "1"},
        #     {"input": "123", "output": "1"}   ],
        # "submit_id": "24", "types": {"in": ["int"], "out": "bool"},
        # "env": {"mem": 10, "proc": 2, "time": 2} },

        # {"name": "itc_ispositive_d", "tests": [
        #     {"input": "123.23", "output": "1"},
        #     {"input": "-1123.123", "output": "0"},
        #     {"input": "0.23", "output": "1"},
        #     {"input": "123.567", "output": "1"}   ],
        # "submit_id": "25", "types": {"in": ["double"], "out": "bool"},
        # "env": {"mem": 10, "proc": 2, "time": 2} }
    ]
}

print(json.dumps(payload))
print("\n" * 5)

payload = {
    "language": "cpp",
    "github_link": "https://github.com/ret7020/CppHeaders.git",
    "test_type": "header_test",
    "tests_description": [
        {
            "name": "itc_abs",
            "tests": [
                {
                    "input": "-34",
                    "output": "34",
                    "demo": True
                },
                {
                    "input": "1009",
                    "output": "1009"
                },
                {
                    "input": "0",
                    "output": "0"
                },
                {
                    "input": "-2147483647",
                    "output": "2147483647"
                }
            ],
            "submit_id": "65710776-ce95-4f19-8cd9-4b9a1430419f",
            "types": {
                "in": [
                    "int"
                ],
                "out": [
                    "int"
                ]
            },
            "env": {
                "mem": 100,
                "proc": 2,
                "time": 2
            }
        },
        # {
        #     "name": "itc_fabs",
        #     "tests": [
        #         {
        #             "input": "-34.78",
        #             "output": "34.00000",
        #             "demo": True
        #         },
        #         {
        #             "input": "34.99",
        #             "output": "34.00000"
        #         },
        #         {
        #             "input": "-3333444234.99",
        #             "output": "2147483647.00000"
        #         },
        #         {
        #             "input": "0.0",
        #             "output": "0.00000"
        #         },
        #         {
        #             "input": "-2147483648234324324324456546.345345435345423899",
        #             "output": "-2147483647.00000"
        #         }
        #     ],
        #     "submit_id": "c5b27948-9b7f-40dc-95b3-6aaba1c306ee",
        #     "types": {
        #         "in": [
        #             "double"
        #         ],
        #         "out": [
        #             "double"
        #         ]
        #     },
        #     "env": {
        #         "mem": 100,
        #         "proc": 2,
        #         "time": 2
        #     }
        # },
        # {
        #     "name": "itc_revnbr",
        #     "tests": [
        #         {
        #             "input": "100",
        #             "output": "1",
        #             "demo": True
        #         },
        #         {
        #             "input": "205",
        #             "output": "502"
        #         },
        #         {
        #             "input": "999",
        #             "output": "999"
        #         },
        #         {
        #             "input": "810",
        #             "output": "18"
        #         },
        #         {
        #             "input": "981",
        #             "output": "189"
        #         },
        #         {
        #             "input": "-123",
        #             "output": "-321"
        #         },
        #         {
        #             "input": "-200",
        #             "output": "-2"
        #         }
        #     ],
        #     "submit_id": "678500e4-053c-49c0-b48d-70fddc8a3d6a",
        #     "types": {
        #         "in": [
        #             "int"
        #         ],
        #         "out": [
        #             "int"
        #         ]
        #     },
        #     "env": {
        #         "mem": 100,
        #         "proc": 2,
        #         "time": 10
        #     }
        # },
        # {
        #     "name": "itc_iseven",
        #     "tests": [
        #         {
        #             "input": "100",
        #             "output": "1",
        #             "demo": True
        #         },
        #         {
        #             "input": "205",
        #             "output": "0"
        #         },
        #         {
        #             "input": "2147483647",
        #             "output": "0"
        #         },
        #         {
        #             "input": "-2147483648",
        #             "output": "1"
        #         },
        #         {
        #             "input": "0",
        #             "output": "1"
        #         }
        #     ],
        #     "submit_id": "838b4a82-2588-4f3b-842e-312ff06b9eb2",
        #     "types": {
        #         "in": [
        #             "int"
        #         ],
        #         "out": [
        #             "bool"
        #         ]
        #     },
        #     "env": {
        #         "mem": 100,
        #         "proc": 2,
        #         "time": 2
        #     }
        # },
        # {
        #     "name": "itc_max",
        #     "tests": [
        #         {
        #             "input": "100\n100",
        #             "output": "100",
        #             "demo": True
        #         },
        #         {
        #             "input": "-23\n2147483647",
        #             "output": "2147483647"
        #         },
        #         {
        #             "input": "-2147483648\n2147483647",
        #             "output": "2147483647"
        #         },
        #         {
        #             "input": "0\n0",
        #             "output": "0"
        #         },
        #         {
        #             "input": "89\n2",
        #             "output": "89"
        #         }
        #     ],
        #     "submit_id": "17533c43-4ca3-428e-9fd1-ee198f6584e3",
        #     "types": {
        #         "in": [
        #             "int",
        #             "int"
        #         ],
        #         "out": [
        #             "int"
        #         ]
        #     },
        #     "env": {
        #         "mem": 100,
        #         "proc": 2,
        #         "time": 2
        #     }
        # }
    ]
}


print(json.dumps(payload))
result = requests.post("http://localhost:7777/homework", data=json.dumps(payload), timeout=10000)
print(result.text)