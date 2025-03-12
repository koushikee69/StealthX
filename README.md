# StealthX-Dynamic Library Hijacking & Security Analysis

StealthX is a research project focused on **Linux dynamic linking and process hiding** using `LD_PRELOAD`. This project explores methods for manipulating shared libraries to intercept system calls, providing insights into cybersecurity, stealth techniques, and process injection.

## 🚀 Project Overview
StealthX demonstrates how shared object (.so) files can be used to override system functions, potentially hiding processes or modifying program behavior dynamically. It serves as an educational tool for security researchers and ethical hackers interested in low-level Linux internals.

## 🔬 Technologies Used
- **Programming Language**: C
- **Linux System Calls**: execve, open, fopen, ptrace
- **Networking**: Reverse shell execution via sockets
- **Dynamic Linking**: LD_PRELOAD for function interception

## 🔧 Features
- **Intercept system calls dynamically** using `LD_PRELOAD`
- **Modify process visibility** in system utilities (`ps`, `top`, `ls`, etc.)
- **Explore Linux shared library injection techniques**
- **Study stealth methods** for red teaming and cybersecurity analysis
- **Educational purposes only** – **not for malicious use**

## 🔍 How It Can Help
- **Security Researchers**: Understand and analyze Linux process injection and stealth techniques.
- **Ethical Hackers & Red Teamers**: Learn how attackers might use `LD_PRELOAD` for stealth and how to detect such techniques.
- **Developers**: Gain insights into dynamic linking and runtime library manipulation.
- **Students & Enthusiasts**: Experiment with shared library hijacking in a controlled environment.

## 🛠 Installation
Ensure you have **GCC** installed on your Linux machine before proceeding.

```bash
sudo apt update && sudo apt install build-essential
```

### 1️⃣ Clone the Repository
```bash
git clone https://github.com/koushikee69/StealthX.git
cd stealthx
```

### 2️⃣ Compile the Shared Library
```bash
gcc -shared -o StealthX.so stealthx.c -fPIC
```

### 3️⃣ Move the Shared Library to System Directory
```bash
sudo mv StealthX.so /usr/lib/
```

### 4️⃣ Modify `ld.so.preload` to Load the Library
```bash
sudo touch /etc/ld.so.preload
sudo chmod 644 /etc/ld.so.preload
echo "/usr/lib/StealthX.so" | sudo tee -a /etc/ld.so.preload
```

## 🚨 Disclaimer
This project is **strictly for educational and research purposes**. Unauthorized use of stealth techniques can be **illegal**. Ensure compliance with ethical guidelines and legal frameworks.

## 📌 Contribution
Contributions are welcome! Feel free to fork, submit pull requests, or suggest improvements.

## 📄 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

