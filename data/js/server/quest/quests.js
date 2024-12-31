function QuestList(questID)
{
	var questList = {
		"1": {
			title: "Collect Fruits",
			description: "Collect 2 apples, 3 oranges, and 1 banana.",
			uncomplete: "You have not collected all the fruits yet.",
			complete: "You have collected all the fruits! Here is your reward.",
			refuse: "You have refused to collect the fruits.",
			type: "collect",
			questTurnIn: 1, // 0 = auto-complete
			targetItems: [
				{ itemID: "0x13e3", amount: 2, name: "Apples" }, // Apples
				{ itemID: "0x13e4", amount: 3, name: "Oranges" }, // Oranges
				{ itemID: "0x13e5", amount: 1, name: "Bananas" }  // Bananas
			],
			rewards: [
				{ type: "gold", amount: 500, name: "Gold Coins" },
				{ type: "item", itemID: "0x1f4c", amount: 1, name: "Magical Scroll" },
				{ type: "karma", amount: 50, name: "Karma Points" },
				{ type: "fame", amount: 30, name: "Fame Points" }
			]
		},
		"2": {
			title: "Hunt Dangerous Creatures",
			description: "Kill 1 dog and 1 cat.",
			type: "kill",
			targetKills: [
				{ npcID: "dog", amount: 1 }, // Kill 5 orcs
				{ npcID: "cat", amount: 1 }  // Kill 3 cats
			],
			rewards: [
				{ type: "gold", amount: 1000 },
				{ type: "karma", amount: 50 },
				{ type: "fame", amount: 30 }
			]
		},
		"3": {
			title: "Collect Fruits",
			type: "collect",
			targetItems: [
				{ itemID: "0x13e3", amount: 2 }, // Apples
			],
			rewards: [
				{ type: "gold", amount: 500 }, // Give 500 gold
				{ type: "item", itemID: "0x1f4c", amount: 1 }, // Give a magical scroll
				{ type: "karma", amount: 50 }, // Add 50 karma
				{ type: "fame", amount: 30 }   // Add 30 fame
			]
		},
		"4": {
			title: "Gather Fruits and Hunt Animals",
			type: "multi", // New type for combined objectives
			targetItems: [
				{ itemID: "0x13e3", amount: 1 } // Collect 3 apples
			],
			targetKills: [
				{ npcID: "dog", amount: 1 }, // Kill 1 dog
			],
			rewards: [
				{ type: "gold", amount: 500 }, // Give 500 gold
				{ type: "item", itemID: "0x1f4c", amount: 1 }, // Give a magical scroll
				{ type: "karma", amount: 50 }, // Add 50 karma
				{ type: "fame", amount: 30 }   // Add 30 fame
			]
		},
		"5": {
			title: "Timed Fruit Collection",
			type: "timecollect",
			targetItems: [
				{ itemID: "0x13e3", amount: 5 } // Collect 5 apples
			],
			timeLimit: 300, // Time limit in seconds
			rewards: [
				{ type: "gold", amount: 100 },
				{ type: "karma", amount: 10 }
			]
		},
		"6": {
			title: "Timed Hunt Animals",
			type: "timekills",
			targetKills: [
				{ npcID: "dog", amount: 1 }, // Kill 1 dog
			],
			timeLimit: 100, // Time limit in seconds
			rewards: [
				{ type: "gold", amount: 100 },
				{ type: "karma", amount: 10 }
			]
		},
		"7": {
			title: "Gather Apples",
			description: "Collect 10 apples from the orchard.",
			type: "collect",
			questTurnIn: 1, // 0 = auto-complete
			targetItems: [{ itemID: "0x13e3", amount: 1, name: "Apple" }],
			rewards: [{ type: "gold", amount: 500 }],
			nextQuestID: "8" // Chain to the next quest
		},
		"8": {
			title: "Defeat the Bandits",
			description: "Defeat 5 bandits in the nearby forest.",
			type: "kill",
			questTurnIn: 1, // 0 = auto-complete
			targetKills: [{ npcID: "cat", amount: 1 }],
			rewards: [{ type: "gold", amount: 1000 }],
			nextQuestID: "9" // Chain to the next quest
		},
		"9": {
			title: "Deliver Supplies to the Guard",
			description: "Bring the supplies to the guard at the eastern gate.",
			type: "delivery",
			targetDelivery: { npcID: "nail", location: { x: 1000, y: 2000, z: 0 } },
			deliveryItem: { itemID: "0x1F4C", amount: 1 },
			rewards: [
				{ type: "gold", amount: 1000 },
				{ type: "item", itemID: "0x1B7A", amount: 1 }
			],
			nextQuestID: null
		},
		"10": {
			"title": "Deliver the Package to Nail",
			"type": "delivery",
			"description": "Deliver the package to Nail, the receiver, located at his usual spot.",
			"deliveryItem": {
				"itemID": "0x0E21", // Package item ID
				"name": "Sealed Package", // Friendly name for the item
				"amount": 1 // Quantity
			},
			"targetDelivery": {
				"sectionID": "nail", // Section ID of Nail
				"name": "Nail the Receiver", // Name of the NPC
				"location": {
					"x": 1234, // X coordinate
					"y": 5678, // Y coordinate
					"z": 0,    // Z coordinate
					"world": 1 // World number
				}
			},
			"rewards": [
				{ "type": "gold", "amount": 500 },
				{ "type": "item", "itemID": "0x1F4C", "amount": 1 }
			]
		}
	};
	// Return the full quest list if no questID is provided
	if (!questID)
	{
		return questList;
	}

	// Return a specific quest if questID is provided
	return questList[questID] || null;
}