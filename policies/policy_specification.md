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
        /ATTRIBUTE/1
        /ATTRIBUTE/2
        /ATTRIBUTE/3
        ...
    }
    deny
    {
        /ATTRIBUTE/4
        /ATTRIBUTE/5
        /ATTRIBUTE/6
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
requester-names
    FUNCTION
        specifies what users the access control policy should apply to
    TYPE
        list of ndn-formatted names separated by spaces, commas, or both
        list should be surrounded by quotes
    NOTE:
        incorrect formatting will result in inacurate outputs without warning from parser
attribute-filters
    NOTE: stream names are treated as attributes
    FUNCTION
        allows for access control on the attribute level
        specifies which attributes data requesters should be allowed or denied
    REQUIREMENTS
        must have "allow" section with at least one attribute
        cannot allow and deny the same attribute
    TYPE
        following the attribute naming format, each line within the "allow" and "deny" fields names a single attribute
```

Comments
```
DENY ALL ACCESS
ALLOW FROM GIVEN STREAMS
DENY FROM ALL BUT VALUE
DENY ALL ACCESS
ALLOW ALL TIME ACCORDING TO VALUE WITHIN GIVEN STREAMS
```
