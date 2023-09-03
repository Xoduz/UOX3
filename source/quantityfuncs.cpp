
#include "cchar.h"
#include "citem.h"
#include "funcdecl.h"

#include "weight.h"

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetSubTotalItemCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of items in a container
// o------------------------------------------------------------------------------------------------o
auto GetSubTotalItemCount(CItem *objCont) -> std::uint32_t {
    std::uint32_t total = 0;
    auto pCont = objCont->GetContainsList();
    for (const auto &i : pCont->collection()) {
        if (ValidateObject(i)) {
            if (i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER) {
                total += 1; // Also count the container
                total += GetSubTotalItemCount(i);
            }
            else {
                if (!(i->GetLayer() == IL_FACIALHAIR || i->GetLayer() == IL_HAIR)) {
                    total += 1;
                }
            }
        }
    }
    return total;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetTotalItemCount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of items in a container
// o------------------------------------------------------------------------------------------------o
std::uint32_t GetTotalItemCount(CItem *objCont) {
    if (!ValidateObject(objCont) ||
        (objCont->GetType() != IT_CONTAINER && objCont->GetType() != IT_LOCKEDCONTAINER))
        return 0;

    return GetSubTotalItemCount(objCont);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetSubItemAmount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item in a pack
// o------------------------------------------------------------------------------------------------o
auto GetSubItemAmount(CItem *p, std::uint16_t realId, std::uint16_t realColour, std::uint32_t realMoreVal,
                      bool colorCheck = false, bool moreCheck = false, std::string sectionId = "")
    -> std::uint32_t {
    std::uint32_t total = 0;
    auto pCont = p->GetContainsList();
    for (const auto &i : pCont->collection()) {
        if (ValidateObject(i)) {
            if (i->GetId() != realId &&
                (i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER)) {
                total += GetSubItemAmount(i, realId, realColour, realMoreVal, colorCheck, moreCheck,
                                          sectionId);
            }
            else if (i->GetId() == realId &&
                     (!colorCheck || (colorCheck && i->GetColour() == realColour)) &&
                     (!moreCheck || (moreCheck && i->GetTempVar(CITV_MORE) == realMoreVal)) &&
                     (sectionId == "" || sectionId == i->GetSectionId())) {
                if (i->GetUsesLeft() > 0) {
                    total += i->GetUsesLeft();
                }
                else {
                    total += i->GetAmount();
                }
            }
        }
    }
    return total;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetItemAmount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item on a character
// o------------------------------------------------------------------------------------------------o
std::uint32_t GetItemAmount(CChar *s, std::uint16_t realId, std::uint16_t realColour, std::uint32_t realMoreVal, bool colorCheck,
                   bool moreCheck, std::string sectionId) {
    CItem *p = s->GetPackItem();
    if (!ValidateObject(p))
        return 0;

    return GetSubItemAmount(p, realId, realColour, realMoreVal, colorCheck, moreCheck, sectionId);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	DeleteSubItemAmount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a certain amount of an item of specified color in a pack
// o------------------------------------------------------------------------------------------------o
auto DeleteSubItemAmount(CItem *p, std::uint32_t amount, std::uint16_t realId, std::uint16_t realColour, std::uint32_t realMoreVal,
                         bool colorCheck, bool moreCheck, std::string sectionId) -> std::uint32_t {
    if (!ValidateObject(p))
        return 0;

    std::uint32_t amtDeleted = 0;
    std::uint32_t total = amount;
    auto pCont = p->GetContainsList();
    auto collection = pCont->collection(); // force a copy, we will be deleting
    for (auto &i : collection) {
        if (!ValidateObject(i))
            continue;

        if (i->GetId() != realId &&
            (i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER)) {
            // Is item an pack or container?
            amtDeleted += DeleteSubItemAmount(i, total, realId, realColour, realMoreVal, colorCheck,
                                              moreCheck, sectionId);
        }
        else if (i->GetId() == realId &&
                 (!colorCheck || (colorCheck && i->GetColour() == realColour)) &&
                 (!moreCheck || (moreCheck && i->GetTempVar(CITV_MORE) == realMoreVal)) &&
                 (sectionId == "" || sectionId == i->GetSectionId())) {
            std::uint16_t usesLeft = i->GetUsesLeft();
            if (usesLeft > 0) {
                // If item has uses left, but not enough to cover the total resource cost...
                if (usesLeft <= total) {
                    // ...deplete remaining uses from total and delete item
                    amtDeleted += usesLeft;
                    i->Delete();
                }
                else {
                    // Otherwise, reduce amount of uses left on item
                    i->SetUsesLeft(usesLeft - total);
                }
            }
            else {
                // There are no uses on item, but there might be a stack of the item, or multiple
                // items
                if (i->GetAmount() <= total) {
                    amtDeleted += i->GetAmount();
                    i->Delete();
                }
                else {
                    i->IncAmount(-(static_cast<std::int32_t>(total)));
                    amtDeleted += total;
                }
            }
        }

        if (amtDeleted >= amount) {
            break;
        }
        else {
            total = static_cast<std::uint32_t>(amount - amtDeleted);
        }
    }

    return amtDeleted;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	DeleteItemAmount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a certain amount of an item of specified color on a character
//|
//|	Changes		-	09/24/2002	-	Resource calculations fixed.
//|
//|	Changes		-	09/25/2002	-	Weight Fixes
// o------------------------------------------------------------------------------------------------o
std::uint32_t DeleteItemAmount(CChar *s, std::uint32_t amount, std::uint16_t realId, std::uint16_t realColour, std::uint32_t realMoreVal,
                      bool colorCheck, bool moreCheck, std::string sectionId) {
    if (!ValidateObject(s))
        return 0;

    CItem *p = s->GetPackItem();
    if (!ValidateObject(p))
        return 0;

    return DeleteSubItemAmount(p, amount, realId, realColour, realMoreVal, colorCheck, moreCheck,
                               sectionId);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	GetBankCount()
//|	Date		-	October 23rd, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Searches through the bank to count the amount of items with a
// specific ID and colour
// o------------------------------------------------------------------------------------------------o
std::uint32_t GetBankCount(CChar *p, std::uint16_t itemId, std::uint16_t colour, std::uint32_t moreVal) {
    if (!ValidateObject(p))
        return 0;

    std::uint32_t goldCount = 0;
    for (auto &oItem : *p->GetOwnedItems()) {
        if (ValidateObject(oItem)) {
            if (oItem->GetType() == IT_CONTAINER && oItem->GetTempVar(CITV_MOREX) == 1) {
                goldCount += GetSubItemAmount(oItem, itemId, colour, moreVal, false);
            }
        }
    }
    return goldCount;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	DeleteBankItem()
//|	Date		-	October 23rd, 2000
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Searches through the bank to and deletes a certain amount of a
// certain item |					Returns how many left over
// o------------------------------------------------------------------------------------------------o
std::uint32_t DeleteBankItem(CChar *p, std::uint32_t amt, std::uint16_t itemId, std::uint16_t colour, std::uint32_t moreVal) {
    if (!ValidateObject(p))
        return amt;

    for (auto &oItem : *p->GetOwnedItems()) {
        if (ValidateObject(oItem)) {
            if (oItem->GetType() == IT_CONTAINER && oItem->GetTempVar(CITV_MOREX) == 1) {
                amt -= DeleteSubItemAmount(oItem, amt, itemId, colour, moreVal);
                if (amt == 0)
                    return 0;
            }
        }
    }
    return amt;
}
