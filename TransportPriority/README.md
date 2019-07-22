# CycloneDDS hello-world example

``hello-world`` is a simple cdds example using one data writer and one data reader. 

It use the default domain and publish/subscribe to the ``HelloWorldData_Msg`` topic.

The message type is HelloWorldData:

```idl
module HelloWorldData
{
  struct Msg
  {
    long userID;
    string message;
  };
#pragma keylist Msg userID
};
```

Also, before data writer publishes the ``HelloWorldData`` to the  topic, it will blocking until successfully diccover a matched data reader in the same domain.