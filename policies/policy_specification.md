This document describes the mGuard policy language specifications.

Current Policy Structure
------------------------
```
policy-id             id
requester-names       requester-names
attribute-policy-1
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
attribute-policy-2
{
    allow
    {
        /attribute/7
        /attribute/8
        /attribute/9
        ...
    }
    deny
    {
        /attribute/10
        /attribute/11
        /attribute/12
        ...
    }
}
```

Specification Detail
-----------------------
```
GLOBAL OPTIONS          REQUIRED    TYPE
policy-id               *           int
requester-names         *           "ndn-name, ndn-name, ..."
attribute-policies      1+

ATTRIBUTE-POLICIES      REQUIRED    TYPE
allow                   *           list of attributes in NDN-name format
deny                                list of attributes in NDN-name format
```
```
requester-names
    FUNCTION
        specifies what users the access control policy should apply to
    DATA TYPE
        list of ndn-formatted names separated by spaces, commas, or both
        list should be surrounded by quotes
        if just a single name, no quotes are needed
    NOTE:
        incorrect formatting will result in inacurate outputs without warning from parser
        
attribute-policies
    NOTE: stream names are treated as attributes
    FUNCTION
        allows for access control on the attribute level
        specifies which attributes data requesters should be allowed or denied
    REQUIREMENTS
        must have "allow" section with at least one stream name
    TYPE
        each line within the "allow" and "deny" fields corresponds to a single attribute
```

Policy details
-------------

the labels of attribute-policy-1, attribute-policy-2, etc. can be anything you want

ABE Policies are created for each of these sections independently and then combines them with OR, which is effectively the same as having separate policies with the same requester-names.
Combining these into a single policy allows for all specifications for a group of requesters to be written in a single policy.

Technical details
----
**Definitions**

"allowing" or "denying" an attribute refers to putting that attribute inside an "allow" or "deny" section.
Stream names and attributes are different in that attributes are data attributes unrelated to the stream name.

Stream names may be stream name prefixes.
For example, if a data stream is named `/one/two/three`, a prefix may be `/one/` or `/one/two/`.
Thinking about this as a tree, starting from the "root" prefix, "leaves" of a prefix refer to all data streams which have that prefix.
In this example, if there exists a stream `/one/two/four`, the leaves of prefix `/one/two/` are `/one/two/three` and `/one/two/four`.

Attributes are in the format `/.../ATTRIBUTE/<type>/<value>`. 
For example, if data was created at the gym, it would have the `/.../ATTRIBUTE/location/gym` attribute.

When I refer to a set of names being added to the ABE policy, this means all elements in the set are concatenated with OR and then appended to the ABE policy with AND.
For example, if the existing ABE policy is `stream1`, adding the set of elements `["one", "two"]` results in `stream1 AND ("one" OR "two")`

**Assumptions and Implications**

The parser assumes that any single data packet is encrypted with exactly one attribute from each of the known types. 
Therefore, if data is encrypted with new types, keys from new policies that allow or deny attributes from these types will deny access to previous data not encrypted with attributes of these new types.

**Calculations** 

Note: *These are done separately for each individual "attribute-policy" section*

Allowed attributes are added to the policy.

Denied attributes of the same type are negated from the set of all attributes of that type and then added to the ABE policy.

All denied streams are negated from the set of all leaves of all allowed streams.
The leaves (data stream names) in the resulting set are added to the ABE policy

All time-based attributes are individually appended to the ABE policy with AND.

Comments
------------
```
DENY ALL ACCESS
ALLOW FROM GIVEN STREAMS
DENY FROM ALL BUT VALUE
DENY ALL ACCESS
ALLOW ALL TIME ACCORDING TO VALUE WITHIN GIVEN STREAMS
```
