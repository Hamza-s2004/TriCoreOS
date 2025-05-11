# TriCoreOS

**Short Description:** A suite of essential operating system utilities written in C, showcasing core OS concepts like process control, file management, and basic system calls.

**Authors:**
- Hamza Shabbir (23F-0546)
- Haider Abbas (23F-0632)
- Muhammad Hassan (23F-0676)

## Project Overview
TriCoreOS is a collection of simple command-line utilities implemented in C to demonstrate fundamental operating system mechanisms. This project serves both as an educational toolkit and a foundation for further OS experimentation.

## Utilities Included
| Program            | Description                                 |
|--------------------|---------------------------------------------|
| `calculator`       | Basic arithmetic calculator (add, sub, mul, div) |
| `calendar`         | Displays a simple monthly calendar         |
| `copy`             | Copies a file from source to destination    |
| `createFile`       | Creates a new empty file                    |
| `deleteFile`       | Deletes a specified file                    |
| *(add any other utilities here)*            |

## Directory Structure
```

TriCoreOS/
├── src/                 # Source code files (.c, .h)
├── bin/                 # Compiled executables
├── build/               # Build artifacts
├── docs/                # Additional documentation
├── .gitignore           # Git ignore file
├── LICENSE              # License file
└── README.md            # Project overview and instructions

````

## Prerequisites
- GCC (or compatible C compiler)
- Make (optional)

## Building the Project
From the project root:

```bash
# Option 1: Compile each utility manually
gcc src/calculator.c -o bin/calculator
gcc src/calendar.c   -o bin/calendar
# ... repeat for other tools

# Option 2: Using Makefile (if provided)
make all
````

## Usage

To use the utilities, compile and then run the main executable:

````bash
./bin/main
```bash
# Run the calculator
./bin/calculator

# Show the current month's calendar
./bin/calendar 2025 5

# Copy a file
./bin/copy input.txt output.txt

# Create a new file
./bin/createFile newfile.txt

# Delete a file
./bin/deleteFile oldfile.txt
````

## Contribution

Feel free to fork this repository, submit issues, or open pull requests to improve these utilities.

```
--------------------|---------------------------------------------|
| `calculator`       | Basic arithmetic calculator (add, sub, mul, div) |
| `calendar`         | Displays a simple monthly calendar         |
| `copy`             | Copies a file from source to destination    |
| `createFile`       | Creates a new empty file                    |
| `deleteFile`       | Deletes a specified file                    |
| ...                | *(add any other utilities here)*            |

## Directory Structure
```

OS-project/
├── src/                 # Source code files (.c, .h)
├── bin/                 # Compiled executables
├── build/               # Build artifacts
├── docs/                # Additional documentation
├── .gitignore           # Git ignore file
├── LICENSE              # License file
└── README.md            # Project overview and instructions

````

## Prerequisites
- GCC (or compatible C compiler)
- Make (optional)

## Building the Project
From the project root:

```bash
# Option 1: Compile each utility manually
gcc src/calculator.c -o bin/calculator
gcc src/calendar.c   -o bin/calendar
# ... repeat for other tools

# Option 2: Using Makefile (if provided)
make all
````

## Usage Examples

```bash
# Run the calculator
./bin/calculator

# Show the current month's calendar
./bin/calendar 2025 5

# Copy a file
./bin/copy input.txt output.txt

# Create a new file
./bin/createFile newfile.txt

# Delete a file
./bin/deleteFile oldfile.txt
```

## Contribution

Feel free to fork this repository, submit issues, or open pull requests to improve these utilities.

````


## .gitignore

```gitignore
# Object files
*.o

# Executables
bin/*

# Build directory
build/

# Mac OS
.DS_Store
````

## LICENSE (MIT)

```text
MIT License

Copyright (c) 2025 Hamza Shabbir, Haider Abbas, Muhammad Hassan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

*Copy each block into its respective file (`README.md`, `.gitignore`, `LICENSE`) at the project root.*
