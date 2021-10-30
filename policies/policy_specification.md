This document describes the mGuard policy language specifications.

Current Policy Structure
------------------------
```
policy-id             id
requester-names       requester-names
attribute-filters
{
    allow
    {
        /attribute/1
        /attribute/2
        /attribute/3
        ...
    }
    deny
    {
        /attribute/4
        /attribute/5
        /attribute/6
        ...
    }
}

```

Specification Detail
-----------------------
```
GLOBAL OPTIONS      REQUIRED  TYPE
policy-id           *         int
requester-names     *         "ndn-name, ndn-name, ..."
attribute-filters   *

ATTRIBUTE-FILTERS   REQUIRED
allow               *
deny
```
```
attribute-filters
    NOTE: stream names are treated as attributes
    FUNCTION
        allows for access control on the attribute level
        specifies which attributes data requesters should be allowed or denied
    REQUIREMENTS
        must have "allow" section with at least one attribute
    TYPE
        lines of attributes that follow the attribute naming format
```

Comments
```
DENY ALL ACCESS
ALLOW FROM GIVEN STREAMS
DENY FROM ALL BUT VALUE
DENY ALL ACCESS
ALLOW ALL TIME ACCORDING TO VALUE WITHIN GIVEN STREAMS
```
