# Well...

### Collect all your code into one single file using Eatler.


### Usage
1. Compile using your favourite compiler (i use gcc).
2. _(Optional)_ Add to PATH to execute from anywhere.
3. Use:
   ```bash
   eatler -e .rs,.toml,.ts,.json -d ./my-project -i target,node_modules,.env,.turbo,dist
    ```
### Flags: 
- e - File extensions to collect (in example above, all .rs .ts .toml and .json files will be collected)
- d - Directory to look into (in example above, do all stuff in my-project directory)
- i - Ignored names (in example above, all target, node_modules, .env, .turbo and dist will **NOT** be collected)
