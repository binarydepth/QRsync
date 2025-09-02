# **QRsync**

A modern, cross-platform Qt GUI for the powerful rsync command-line utility. QRsync provides a user-friendly interface for building and managing complex rsync commands, complete with presets and advanced options.

## **Features**

* **Intuitive Interface**: Easy-to-use controls for source, destination, and common rsync options.  
* **Syncset Management**: Save and load your synchronization settings as named "Syncsets" for quick reuse.  
  * New, Save (Overwrite), Load, Rename, and Delete functionality.  
* **Flexible Sync Modes**:  
  * **Contents Mode**: Copies the contents of a directory (source/).  
  * **Mirror Mode**: Copies the directory itself (source).  
* **Granular Archive Control**: Use the simple "Archive (-a)" option or fine-tune individual flags like permissions, times, and symlink handling.  
* **Manual Override**: An expert mode that unlocks the UI's logic, allowing for any combination of rsync flags.  
* **Live Command Preview**: The application shows you the exact rsync command that will be executed.  
* **Integrated Help**: View the rsync manual page directly within the application.

## **Building from Source**

This project uses CMake and Qt6. The following instructions are for Debian-based Linux distributions like Deepin or Ubuntu.

### **1\. Install Dependencies**

Open a terminal and install the necessary development packages:

sudo apt update  
sudo apt install build-essential cmake qt6-base-dev rsync

### **2\. Compile**

1. Navigate to the project's root directory.  
2. Create a build directory:  
   cmake \-B build \-DCMAKE\_BUILD\_TYPE=Release

3. Compile the project:  
   cmake \--build build

4. Run the executable:  
   ./build/QRsync

## **Contributing**

Contributions are welcome! If you find a bug or have feedback, please open a discussion or pull request on the project's repository.

## **License**

CPSL v0.1