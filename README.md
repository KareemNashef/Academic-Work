# Technion Computer Science - Academic Portfolio

A curated collection of substantial coursework projects completed during my B.Sc. in Computer Science at Technion - Israel Institute of Technology.

## 📚 About

This repository showcases challenging assignments that demonstrate core competencies in systems programming, compiler design, algorithms, and software engineering. Each project represents hands-on implementation of fundamental CS concepts, from low-level assembly to high-level language design.

## 🎓 Course Projects

### [Theory of Compilation](https://github.com/KareemNashef/Academic-Work/tree/3b1596a581052a4f7927401cd805973a27c0abeb/02360360%20-%20Theory%20of%20Compilation) 
**Course:** 236360  
**Language:** C++, Flex, Bison  

Built a complete compiler from scratch that translates C++ source code to assembly:
- **Lexical Analysis** - Implemented `.lex` scanner for tokenization
- **Syntax Analysis** - Created `.ypp` parser with Bison for grammar rules
- **Semantic Analysis** - Built symbol table for type checking and scope management
- **Code Generation** - Translated high-level constructs to x86 assembly
- **Optimization** - Implemented basic compiler optimizations

**Key Learning:** Understanding how programming languages work at the deepest level, from source code to machine instructions.

---

### [Introduction to Artificial Intelligence](https://github.com/KareemNashef/Academic-Work/tree/3b1596a581052a4f7927401cd805973a27c0abeb/02360501%20-%20Introduction%20to%20Artificial%20Intelligence)
**Course:** 236501  
**Language:** Python  

#### HW1: Search Algorithms & Heuristics
- Breadth-First Search Graph (BFSG) agent implementation
- Custom heuristic function design and evaluation
- Greedy Best-First Search agent
- Weighted A* algorithm with configurable weights
- A* search with admissible heuristics

#### HW2: Adversarial Search (Game AI)
- Two-player warehouse robot competition game
- Minimax algorithm with depth-limited search
- Alpha-Beta pruning optimization
- Expectimax for probabilistic opponent modeling
- Advanced heuristic design for strategic gameplay

#### HW3: Reinforcement Learning
- Markov Decision Process (MDP) implementation
- Custom environment simulator
- Reward shaping and penalty structure design
- Value iteration and policy optimization
- RL agent training and evaluation

**Key Learning:** Practical implementation of AI algorithms from search to learning, including both deterministic and probabilistic decision-making.

---

### [Computer Organization and Programming](https://github.com/KareemNashef/Academic-Work/tree/3b1596a581052a4f7927401cd805973a27c0abeb/02340118%20-%20Computer%20Organization%20and%20Programming)
**Course:** 234118  
**Language:** Assembly, C

#### HW1: Assembly Programming Fundamentals
- Low-level arithmetic and logic operations
- Register management and memory addressing
- Control flow with conditional jumps

#### HW2: System Calls & Conventions
- Linux system call interface implementation
- Calling conventions and ABI compliance
- Understanding opcode structure and instruction encoding

#### HW3: Binary Exploitation & Linking
- Reverse engineering with `readelf`
- Custom linker script creation
- Binary manipulation and memory layout control

**Key Learning:** Deep understanding of how software interfaces with hardware, from assembly instructions to system-level programming.

---

### [Operating Systems](https://github.com/KareemNashef/Academic-Work/tree/3b1596a581052a4f7927401cd805973a27c0abeb/02340123%20-%20Operating%20Systems)
**Course:** 234123  
**Language:** C, C++  

#### HW1: Unix Shell Implementation
- Built custom command-line shell from scratch
- Process creation with `fork()` and `exec()`
- Pipe and I/O redirection handling
- Signal management and job control

#### HW2: Linux Kernel Modification
- Modified and rebuilt Linux kernel source
- Added custom system calls
- Kernel-level debugging and testing
- Understanding kernel architecture

#### HW3: Multithreaded Web Server
- Concurrent HTTP server with thread pool
- Thread synchronization with mutexes and semaphores
- Request queue management
- Race condition prevention and deadlock avoidance

#### HW4: Memory Management (malloc/free)
- Custom memory allocator implementation
- Heap management with free lists
- Coalescing adjacent free blocks
- Memory fragmentation optimization

**Key Learning:** Systems programming at the OS level, from user-space applications to kernel modifications.

---

### [Object-Oriented Programming](https://github.com/KareemNashef/Academic-Work/tree/3b1596a581052a4f7927401cd805973a27c0abeb/02360703%20-%20Object-Oriented%20Programming)
**Course:** 236703  
**Languages:** Squeak (Smalltalk), Java, C++  

#### HW1 & HW3: Squeak/Smalltalk
- Advanced object-oriented design in pure OO language
- Dynamic typing and message passing
- Metaprogramming and reflection

#### HW2 & HW4: Java
- Complex object hierarchies and inheritance
- Exception handling and error recovery
- Unit testing frameworks (JUnit)
- Reflection API and runtime type inspection
- Annotations and metadata programming

#### HW5: Advanced C++ & Design Patterns
- Template metaprogramming
- Observer pattern implementation
- Event-driven architecture
- **Compile-time Rush Hour game** - Game logic executed during compilation using template metaprogramming

**Key Learning:** Comparing OOP paradigms across three different languages, from pure OOP to hybrid approaches.

---

### [Data Structures](https://github.com/KareemNashef/Academic-Work/tree/3b1596a581052a4f7927401cd805973a27c0abeb/02340218%20-%20Data%20Structures)
**Course:** 234218  
**Language:** C++  

#### HW1: AVL Tree Implementation
- Self-balancing binary search tree from scratch
- Rotation operations (left, right, left-right, right-left)
- Insertion and deletion with automatic rebalancing
- All operations meeting strict time complexity requirements

#### HW2: Advanced Data Structures
- Enhanced AVL tree with additional functionality
- Hash table with collision resolution (chaining/open addressing)
- Union-Find (Disjoint Set) data structure
- Path compression and union by rank optimizations
- Complex operations with rigorous complexity constraints

**Key Learning:** Implementing fundamental data structures from scratch with mathematical guarantees on performance.

---

## 🛠️ Technologies & Skills Demonstrated

### Languages
- **C** - Low-level systems programming, memory management
- **C++** - OOP, templates, STL, metaprogramming
- **Python** - AI algorithms, rapid prototyping
- **Assembly** - x86/MIPS architecture
- **Java** - Enterprise OOP patterns
- **Squeak/Smalltalk** - Pure object-oriented paradigm
- **Flex/Bison** - Compiler tools

### Core Concepts
- **Systems Programming** - OS internals, kernel development, threading
- **Compiler Construction** - Lexing, parsing, code generation
- **Algorithms** - Search, optimization, graph algorithms
- **Artificial Intelligence** - Search, game AI, reinforcement learning
- **Memory Management** - Manual allocation, pointer manipulation
- **Concurrent Programming** - Multithreading, synchronization, race conditions
- **Data Structures** - Self-balancing trees, hash tables, union-find
- **Software Design** - Design patterns, architecture, OOP principles

### Tools & Environment
- **GCC/G++** - Compilation and debugging
- **GDB/Valgrind** - Memory debugging and profiling
- **Make/CMake** - Build automation
- **Linux** - Development environment and kernel modification
- **Git** - Version control

---

## 🚀 Highlighted Projects

### 1. Complete C++ Compiler (Theory of Compilation)
The most comprehensive project - building a full compiler taught me how programming languages actually work. From scanning source code characters to generating executable assembly, every layer of abstraction became tangible.

### 2. Linux Kernel Modification (Operating Systems)
Directly modifying the Linux kernel provided unparalleled insight into OS architecture. Understanding how system calls traverse from user space to kernel space fundamentally changed how I think about software.

### 3. Compile-Time Rush Hour Game (OOP - C++)
Using C++ template metaprogramming to create a game that runs during compilation demonstrated the power and complexity of modern C++. The entire game logic executes before runtime.

---

## 📝 Code Quality Standards

All code in this repository:
- Passed automated testing suites and rigorous grading
- Meets time and space complexity requirements where specified
- Follows course coding standards and best practices
- Includes clear comments and documentation
- Has been cleaned and standardized for presentation

---

## ⚠️ Academic Integrity Notice

**For Current Students:**  
This repository is shared publicly for **portfolio purposes only**. Copying this code for your own assignments constitutes academic dishonesty and violates Technion's honor code. Academic integrity violations can result in course failure or expulsion. Do not risk your degree.

**For Employers & Recruiters:**  
This work demonstrates my technical foundations and problem-solving abilities. I can:
- Walk through any implementation in detail
- Explain design decisions and algorithmic choices
- Discuss time/space complexity trade-offs
- Demonstrate how these concepts apply to real-world engineering

Feel free to ask about any project during technical interviews.

---

## 🎯 Why This Work Matters

These projects represent more than academic exercises:

- **Systems Thinking** - Understanding how software layers interact from hardware to high-level abstractions
- **Problem-Solving** - Breaking down complex problems into manageable components
- **Engineering Rigor** - Writing code that meets strict performance and correctness requirements
- **Depth of Knowledge** - Going beyond using libraries to implementing core CS concepts from scratch

This foundation enables me to quickly learn new technologies and understand systems at a fundamental level.

---

## 📧 Contact

**Kareem Nashef**
- Email: Kareem.na@outlook.com
- LinkedIn: [linkedin.com/in/kareem-nashef](https://linkedin.com/in/kareem-nashef)
- GitHub: [@KareemNashef](https://github.com/KareemNashef)

---

<div align="center">

**Technion - Israel Institute of Technology**  
*Computer Science B.Sc.*

Built with determination, debugged with patience, optimized with obsession.

</div>
