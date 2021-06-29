This document describes the mGuard policy language specifications.

Current Policy Structure
------------------------

POLICY-ID:          <id>
STUDY-ID:           <sutdy name>
DATA-OWNER-ID:      <participant id>
DATA-REQUESTER-IDs: <[requesters ids]>
DATA-STREAM-NAME:   
DATA-WINDOW:
  ALLOW:
    StreamName: <...--*--GYRO>                      /* allowed stream within the widow */

    HOW TO SPECIFY MULTIPLE COLUMNS AND VALUES

    Value:      [Allowed attributes]    /* e.g. "/attribute/location/work" */
    Value:      {{1900, 2000}, work}


  DENY:
    StreamName: name
    ColumnName: time

DENY ALL ACCESS
ALLOW FROM STREAM-NAME
DENY FROM ALL BUT VALUE

DENY ALL ACCESS
ALLOW ALL TIME ACCORDING TO VALUE WITHIN STREAM-NAME

-----------------------



GLOBAL OPTIONS      REQUIRED  TYPE

POLICY-ID           *         int
STUDY-ID            *         alpha-numeric
DATA-OWNER          *         alpha-numeric
DATA-REQUESTER-IDs  *         {alpha-numeric, alpha-numeric, ...}
DATA-STREAM-NAME    *
DATA-WINDOW            

DATA-STREAM-NAME
  FUNCTION:
    confirms ownerID and studyID
    allows all under specified node only if no ALLOW or DENY
  TYPE : <alpha-numeric with regex>
    regex Allowed
    component SPECS
      separated by --
      only . and _ allowed within name
        <top.level--cell.phone--gyro>   VALID
        <top.level--cell-phone--gyro>   INVALID
    regex allowed on tree level, not within component
      <top.level--*phone--gyro>       INVALID
      <top.level--*--gyro>            VALID 

DATA-WINDOW:
    ALLOW/DENY (all support NONE) not required
      StreamName
        same as DATA-STREAM-NAME but without regex
          
      ColumnName
        allowed:
          alpha-numeric
          _
        disallow:
          string regex

      ColumnNameValue
        format
          {ColumnName, {Value}}
        ColumnName:
          same as ColumnName above
        Value:
          TODO: specify format depending on type of data, EX timestamp
          allowed formats:
            {alpha-numeric and _, alpha-numeric and _, ...}
            {startTimestamp, endTimestamp} 

for meeting:
  ask about timestamp format
  discuss ColumnNameValue concept
