#include "../Core/Terrain/WorldLoader.h"

list<NetworkS*> deleteClient;

void initalize() {
  initMapgen();
}

int verifyVersion(DataElement* data) {
  if (data->_children.size() != 4) {
    return LoginErrorProtocolError;
  }
  if (data->_children[0]->_core->toType<int>() != VersionA) {
    return LoginErrorVersionError;
  }
  if (data->_children[1]->_core->toType<int>() != VersionB) {
    return LoginErrorVersionError;
  }
  return LoginErrorOk;
}

void sendLoginReject(NetworkS* from, string message, int state) {
  DataElement* res = new DataElement();

  DataElement* statee = new DataElement();
  vGFunc(state, statee);
  res->addChild(statee);

  DataElement* vera = new DataElement();
  vGFunc(VersionA, vera);
  res->addChild(vera);

  DataElement* verb = new DataElement();
  vGFunc(VersionB, verb);
  res->addChild(verb);

  DataElement* verc = new DataElement();
  vGFunc(VersionC, verc);
  res->addChild(verc);
   
  DataElement* verm = new DataElement();
  vGFunc(message, verm);
  res->addChild(verm);

  from->SendData(res, PacketLogin);
  deleteClient.push_back(from);
}

bool recivePacket(DataElement* Data, int Id, NetworkS* thisptr, NetBinder* connected) {
  if (connected == NULL) {
    if (Id != PacketLogin) {
      sendLoginReject(thisptr, "Protocol error!", LoginErrorProtocolError);
      return true;
    }
    int res = verifyVersion(Data);
    if (res != LoginErrorOk) {
      sendLoginReject(thisptr, "Version error!", res);
      return true;
    }

    guid_t guid;
    vSFunc(guid, Data->_children[3]);
    if (guid % 2) {
      NetBinder* np = new NetBinder(guid);
      np->loadFile();
      thisptr->ConnectedBinder = connected = np;
      np->connection = thisptr;

      DataElement* rese = new DataElement();

      DataElement* statee = new DataElement();
      vGFunc(res, statee);
      rese->addChild(statee);

      DataElement* ente = new DataElement();
      oGFunc(*np, ente);
      rese->addChild(ente);

      thisptr->SendData(rese, PacketLogin);

      return false;
    }
    sendLoginReject(thisptr, "Invalid GUID!", LoginErrorInvalidAuth);
    return true;
  }
  return connected->recivePacket(Data, Id, thisptr, connected);
}

int main() {
  initalize();

  while (true) {
    NetworkS* newClient = new NetworkS("7743",recivePacket);
  }
  return 0;
}