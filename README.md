## 📖 Project Presentation
**ProjectModernCpp** is a high-performance Client-Server application architected using **Modern C++** standards. It is designed to demonstrate robust network communication, modular software design, and modern GUI implementation.

**Key Technical Features:**
* **Architecture:** Asynchronous Client-Server model utilizing TCP/IP sockets for reliable data transmission.
* **User Interface:** Built with **Qt 6.9.3**, providing a responsive and modern user experience.
* **Data Serialization:** Implements **Google Protocol Buffers (Protobuf)** for efficient, language-neutral, and platform-neutral mechanism for serializing structured data.
* **Dependency Management:** Fully integrated with **vcpkg** for seamless library handling.
* **Modular Design:** Core logic is encapsulated in Dynamic Link Libraries (DLLs) to ensure separation of concerns and reusability.

---

## 🛠️ Prerequisites
To successfully build and run this project, ensure the following tools are installed:

* **Visual Studio 2022 (or 2019):** Workload required is "Desktop development with C++".
* **vcpkg (C++ Package Manager):** Must be the latest version and integrated globally.
* **Qt 6.9.3:** Installed via the Qt Online Installer.
* **Qt Visual Studio Tools:** Extension for Visual Studio (install via `Extensions > Manage Extensions`).

---

## ⚙️ Setup & Configuration
**1. Integrate vcpkg**
Open a terminal (PowerShell or CMD) in your vcpkg installation folder and run the global integration command:
```powershell
.\vcpkg integrate install
```

**2. Configure Qt in Visual Studio**
* Open Visual Studio.
* Navigate to `Extensions > Qt VS Tools > Qt Versions`.
* Add the path to your Qt 6.9.3 installation (e.g., `C:\Qt\6.9.3\msvc2019_64`).

---

## 🔨 Build Instructions

**Step 1: Build TheGameDLL**
You **must** build the **TheGameDLL** project first. This library contains the core logic that the Server depends on to **generate room codes**.

**Step 2: Build Server (The "Double Build" Requirement)**
⚠️ **Note for first run:** When building the Server for the very first time, it is normal for the process to fail.
* **Why:** The first attempt triggers the Protobuf compiler to generate the necessary `.pb.h` and `.pb.cc` files.
* **Action:** Simply right-click the solution and select **Build** again. The second build will succeed because the required files are now present.

**Step 3: Build Client**
Once the Server has been successfully built, you can proceed to build the Client project. 
> **Note:** You can only **run** the Client after completing the **DLL Management** step below.

---

## 🚀 Critical Setup: DLL Management
The Client application will **not start** without specific DLL files generated during the build. These must be manually placed in the Client's directory.

**1. Locate Source DLLs**
After a successful build, navigate to the output directory:
`ProjectModernCpp\Server\x64\Debug`

Identify these two files:
* `abseil_dll.dll`
* `libprotobufd.dll` (The 'd' stands for Debug).

**2. Copy and Rename (For Client)**
Navigate to the Client project folder (the directory containing the Client's `.vcxproj` file).

* Copy `abseil_dll.dll` into this folder.
* Copy `libprotobufd.dll` into this folder.
* **RENAME** `libprotobufd.dll` to `libprotobuf.dll`.

> **⚠️ IMPORTANT:** You must remove the letter **'d'** from the filename. The Client application is specifically configured to look for `libprotobuf.dll`.

---

## ❓ Troubleshooting

| Issue | Solution |
| :--- | :--- |
| **Server build failed first time** | This is expected. Run the **Build** command a second time. |
| **"libprotobuf.dll was not found"** | Check the Client folder. Ensure you renamed `libprotobufd.dll` to `libprotobuf.dll`. |
| **Unresolved external symbol** | Verify that **TheGameDLL** was built before the Server and that `vcpkg integrate install` was successful. |
| **Room code not generating** | Ensure the Server has access to the functions exported by `TheGameDLL`. |
