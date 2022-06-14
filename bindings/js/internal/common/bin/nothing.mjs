#!/usr/bin/env node

// yarn workspace run SCRIPT requires all child packages to define SCRIPT in their package.json. But, package.json
// does not like a script with an empty string. Other than echo, there is no cross-platform no-op command. The
// solution is a program that literally does nothing to be put into child package script values. If a future
// version of yarn fixes or provides an alternative solution, nothing can be removed.
