This document describes the mGuard policy language specifications.

Current Policy Structure
------------------------

POLICY-ID: <id>
STUDY-ID: <sutdy name>
DATA-OWNER-ID: <participant id>
DATA-REQUESTER-IDs: <[requesters ids]>
DATA-WINDOW:
  ALLOW:
    StreamName: <>              /* allowed stream within the widow */
    ColumnName: [*]
    Value: [Allowed attributes]               /* e.g. "/attribute/location/work" */
  DENY:
    StreamName: <>
    ColumnName: <>
    Value: <>