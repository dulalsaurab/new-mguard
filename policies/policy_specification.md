This document describes the mGuard policy language specifications.

Current Policy Structure
------------------------
```
policy-id             id
requester-ids         requester-ids
stream-name           stream-name
attribute-filters
{
    allow
    {
       attribute/1
       attribute/2
       attribute/3
       ...
    }
    deny
    {
        attribute/4
        attribute/5
        attribute/6
        ...
    }
}

```

Specification Detail
-----------------------
```
GLOBAL OPTIONS      REQUIRED  TYPE
policy-id           *         int
requester-ids       *         "alpha-numeric, alpha-numeric, ..."
stream-name         *
attribute-filters
```
```
stream-name
    FUNCTION:
        allows all under specified node only if no ALLOW or DENY
        wildcard is implied at the end of stream
    TYPE : <alpha-numeric with wildcard>
        regex Limited
    COMPONENT specs
        separated by /
        only "." "_" and "-" allowed within name
            top.level/cell.phone/gyro   VALID
            top.level/cell-phone/gyro   VALID
            $top.level/cell_phone/gyro  INVALID
        wildcard not allowed
            top.level/*phone/gyro       INVALID
            top.level/*/gyro            INVALID 
```
```
attribute-filters
    FUNCTION
        allows for access control on an attribute level
        specifies which attributes data requesters should be allowed or denied
        
        NOTE: this is all within the scope of the given STREAM-NAME
    TYPE
        lines of attributes that follow the attribute naming format
```

Comments
```
DENY ALL ACCESS
ALLOW FROM STREAM-NAME
DENY FROM ALL BUT VALUE
DENY ALL ACCESS
ALLOW ALL TIME ACCORDING TO VALUE WITHIN STREAM-NAME
```
