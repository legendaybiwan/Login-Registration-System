# 🔐 Login & Registration System

A secure, console-based user authentication system available in **C++** and **Python**.

![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-3.7%2B-3776AB?logo=python&logoColor=white)

## Features
- Registration function that takes username and password as input
- Validates input format and rejects duplicate usernames
- Stores credentials in a local file with **salted, hashed passwords** — never plain text
- Login function verifies identity against stored credentials
- Clear success/error messages for both registration and login

## Tech Stack
- C++17 / Python 3.7+ (standard library only, no dependencies)

## How to Run

**C++:**
```bash
g++ -std=c++17 -o auth_system task2_login_registration.cpp
./auth_system
```

**Python:**
```bash
python3 task2_login_registration.py
```

## Security Notes
- **Python version:** uses SHA-256 with a random salt (`hashlib` + `secrets`)
- **C++ version:** uses a salted `std::hash`-based mixing scheme, since the C++ standard library has no built-in cryptographic hash without an external library like OpenSSL

⚠️ Neither approach is production-grade cryptography — for real systems, use a vetted library such as OpenSSL, bcrypt, or Argon2.

## ⚠️ Important
Running this program creates a local credentials file (`users.txt` for C++ / `users.json` for Python). Add it to `.gitignore` before pushing to GitHub if you don't want test data committed:
