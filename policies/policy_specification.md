This document describes the mGuard policy language specifications.

Current Policy Structure
------------------------
```
  POLICY-ID             ID
  STUDY-ID              STUDY-NAME
  DATA-OWNER-ID         PARTICIPANT-ID
  DATA-REQUESTER-IDs    REQUESTER-IDs
  DATA-STREAM-NAME      DATA-STREAM-NAME
  ATTRIBUTE-FILTERS
  {
    ALLOW
    {
        ATTRIBUTE/1
        ATTRIBUTE/2
        ATTRIBUTE/3
        ...
    }
    DENY
    {
        ATTRIBUTE/4
        ATTRIBUTE/5
        ATTRIBUTE/6
        ...
    }
}
```

Specification Detail
-----------------------
```
GLOBAL OPTIONS      REQUIRED  TYPE
POLICY-ID           *         int
STUDY-ID            *         alpha-numeric
DATA-OWNER          *         alpha-numeric
DATA-REQUESTER-IDs  *         "alpha-numeric, alpha-numeric, ..."
DATA-STREAM-NAME    *
ATTRIBUTE-FILTERS
```
```
DATA-STREAM-NAME
  FUNCTION:
    confirms ownerID and studyID
    allows all under specified node only if no ALLOW or DENY
  TYPE : <alpha-numeric with wildcard>
    regex Limited
    component SPECS
      separated by --
      only . and _ allowed within name
        <top.level--cell.phone--gyro>   VALID
        <top.level--cell-phone--gyro>   INVALID
    wildcard allowed on tree level, not within component
      <top.level--*phone--gyro>       INVALID
      <top.level--*--gyro>            VALID 
```
```
ATTRIBUTE-FILTERS
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
