{
  "targets": [
    {
      "target_name": "pam",
      "include_dirs": [ "security/pam_appl.h" ],
      "direct_dependent_settings": {
        "linkflags": [ "-lpam", "-lpam_misc" ]
      },
      'link_settings': {
        'libraries': ["-lpam", "-lpam_misc"],
      },

      "conditions": [
		[ "OS=='win'", {
		
		}, { # OS != win
          "cflags": [ "-g", "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE", "-Wall" ]
        }
      ] ],
      "sources": [ "pam.cc" ]
    }
  ]
}
