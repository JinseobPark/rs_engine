
# RSEngine

#![logo](doxy_scripts/logo.jpg)

A Rock & Stone Engine library


## Documentation



## Doxygen document generation instructions

1. Install WSL

    ```
    wsl --install -d debian
    ```

2. Enter WSL & Configure username & password

3. Run update in wsl
    
    ```Bash
    sudo apt update && sudo apt upgrade -y
    ```

4. Install dependencies

    ```Bash
    sudo apt install doxygen graphviz python3 texlive-full make
    ```

5. cd to the script & give execution permission (in case on native linux)

   ```Bash
   cd doxy_scripts
   chmod +x ./*
   ```

6. Run script
   
   ```Bash
   ./gen-doc.sh
   ```
   
7. Check results at [`doxy_output/html/index.html`](doxy_output/html/index.html) and [`refman.pdf`](refman.pdf)
