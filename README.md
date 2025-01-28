# IODD File Parser for IO-Link Sensors  

[![IODD Parser](https://img.shields.io/badge/IO--Link-Sensor%20Parser-blue)](https://github.com/longntv/iodd_parser_cpp)

## Overview  

This project provides a C++ parser for **IODD (IO Device Description)** files, enabling seamless integration and management of IO-Link sensor data. The parser supports various XML-based IODD files and extracts critical information like device identity, process data, menus, and variables.  

## Features  

- **Parse IODD XML files:** Reads and processes IO-Link sensor definitions.  
- **Support for multiple sensor types:** Includes compatibility with various manufacturers (e.g., Balluff, ifm).  
- **Organized structure:** Modular codebase for easy extension and maintenance.  
- **Efficient XML parsing:** Uses `ezxml` for lightweight XML handling.  
- **Cross-platform compatibility:** Build and run on Device, Linux, Windows, or macOS.  

---

## File Structure  

```
├── inc/
│   ├── ezxml.h                  # Lightweight XML parser header
│   ├── iodd_data_type.h         # Data type definitions for IODD
│   ├── iodd_device_identity.h   # Device identity structures
│   ├── iodd_menu.h              # Menu-related parsing
│   ├── iodd_parser.h            # Core parser logic
│   ├── iodd_process_data.h      # Process data extraction
│   ├── iodd_sensor.h            # High-level sensor interface
│   └── types.h                  # Common type definitions
├── ioddfiles/                    # Sample IODD XML files
├── src/
│   ├── ezxml.c                  # XML parser implementation
│   ├── iodd_data_type.cpp       # Data type parsing
│   ├── iodd_device_identity.cpp # Device identity implementation
│   ├── iodd_menu.cpp            # Menu structure parsing
│   ├── iodd_parser.cpp          # Core parsing implementation
│   ├── iodd_process_data.cpp    # Process data handling
│   ├── iodd_sensor.cpp          # Sensor interface implementation
│   └── iodd_variable.cpp        # Variable parsing
└── main.cpp                     
```

---

## Prerequisites  

- **C++11 or later**  
- XML parsing library: `ezxml`  
- A supported compiler: GCC, Clang, MSVC  

---

## Installation  

1. Clone this repository:  

   ```bash
   git clone https://github.com/longntv/iodd_parser_cpp
   cd iodd-file-parser
   ```

2. Build the project:  

   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

3. Run the parser with a sample IODD file:  

   ```bash
   ./iodd_parser
   ```

---

## Usage  

### Parsing an IODD File  

The main entry point is `main.cpp`. You can specify an IODD file to parse and retrieve details such as:  

- Device identity  
- Process data  
- Variables and menus  

Modify the `main.cpp` file to handle your desired IODD processing logic.  

---

## Sample IODD Files  

The `ioddfiles/` directory includes examples like:  

- Balluff: `Balluff-BCM_R15E-002-DI00-20200327-IODD1.1.xml`  
- ifm: `ifm-000215-20161025-IODD1.0.1.xml`  
- Generic sensors: `AH002.xml`, `AL002.xml`, `TR7439.xml`  

---

## Contributing  

Contributions are welcome! Feel free to open issues or submit pull requests for:  

- Adding support for new IODD versions.  
- Optimizing the XML parser.  
- Improving documentation.  

---

## SEO Tags

```html
<!-- Primary Keywords -->
<meta name="keywords" content="IODD parser, IO-Link sensor, C++ XML parser, IODD file reader, industrial automation, XML file handler, ezxml parser">

<!-- Description -->
<meta name="description" content="A lightweight C++ parser for IODD (IO Device Description) files. Supports IO-Link sensors from manufacturers like Balluff and ifm. Perfect for industrial automation.">

<!-- Robots -->
<meta name="robots" content="index, follow">

```

---

## Contact  

For inquiries or support, contact:  
**[Long Nguyen]**  
Email: [longntv@gmail.com]  
GitHub: [Your GitHub Profile](https://github.com/longntv)
