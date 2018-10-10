#include "duk/storedreference.h"

StoredReference::StoredReference(void* ptr, ReferenceStorage* duk)
    : duk(duk)
    , stored_index(duk->StoreReference(ptr))
{
}

StoredReference::~StoredReference()
{
  duk->ClearReference(stored_index);
}
