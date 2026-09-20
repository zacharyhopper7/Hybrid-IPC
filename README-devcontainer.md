# Dev Container Student Guide

This guide helps you set up your development environment inside a **VS Code Dev Container** for activities involving **C programs** and **shell scripts**.

---

## ✅ Prerequisites

1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Install [Docker Desktop](https://www.docker.com/products/docker-desktop)
3. Install these VS Code extensions:
   - `Dev Containers`
   - `Jupyter`

----

## Getting Started

Your project is already configured with:

* A `.devcontainer/` folder:
  
  * `Dockerfile` — sets up a minimal Ubuntu system with tools like `gcc`, `make`, and `bash`.
  * `devcontainer.json` — configures the container behavior and VS Code integration.

To begin:

1. Open this folder in **VS Code**.
2. When prompted, click **"Reopen in Container"**.
3. The container will be built and launched automatically.

---

## Pre-Installed Tools

Your container comes preloaded with:

| Tool                      | Purpose                  |
| ------------------------- | ------------------------ |
| `gcc`                     | Compile C programs       |
| `make`                    | Automate build processes |
| `bash`                    | Run shell scripts        |
| `curl`, `git`, `dos2unix` | Common CLI utilities     |

> The default user inside the container is `vscode`.

---

## 📁 Folder Structure

```plaintext
project-root/
├── .devcontainer/
│   ├── Dockerfile
│   └── devcontainer.json
├── README-devcontainer.md
├── README.md
├── Code/solution
    ├── [your C and .sh files go here]
```

---

## Sample Commands

### Compile and Run a C Program

```bash
gcc hello.c -o hello
./hello
```

### Execute a Shell Script

```bash
chmod +x script.sh
./script.sh
```

---

## Extensions Installed Automatically

The following VS Code extension is preinstalled:

* `C/C++ (ms-vscode.cpptools)` – for syntax highlighting, intellisense, and debugging C code.

---

## Troubleshooting

* If you see `"bash" is not the default terminal` warnings, don't worry. The container forces it correctly via settings.

* If your C or shell files aren't running:
  
  * Make sure they're in the **workspace root**.
  * Use `chmod +x` for shell scripts before executing.

---

## Cleanup

To remove compiled files:

```bash
rm hello  # or any other binary
```

---

## Final Tips

* Use **multiple terminals** if running several scripts/tools at once.
* Use **tasks.json** if you want to automate compilation later.
* Don't forget to **commit your source code**, but skip `bin/` or output files.
