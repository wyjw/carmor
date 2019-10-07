import sys

config.llvm_tools_dir = ""
config.llvm_shlib_ext = ".so"

import lit.llvm
lit.llvm.initialize(lit_config, config)

# test_exec_root: The root path where tests should be run.
config.test_exec_root = os.path.join("/home/shaokai/Documents/Columbia/shield_project/carmor/passes/test")

# Let the main config do the real work.
lit_config.load_config(config, "/home/shaokai/Documents/Columbia/shield_project/carmor/passes/test/lit.cfg.py")
