FILE(REMOVE_RECURSE
  "CMakeFiles/dist"
  "index.html"
  "libsrm-0.1.tar.gz"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/dist.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
