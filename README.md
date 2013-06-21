node-pam
========

A node.js pam auth module.  Super simple.

### Example

```javascript
var pam = require('pam');

...

pam.auth('system-auth', username, password, function(result) { 
    if(result) {
        success();
    }
    else {
        fail();
    }
});
