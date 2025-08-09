// User table
Table User {
    id: bigint [primary_key, auto_increment];
    username: varchar [unique, not_null, max_length=50];
    email: varchar [unique, not_null, max_length=100];
    password_hash: varchar [not_null, max_length=255];
    is_active: bool [not_null, default=true];

    // Relationships
    groups: Group [many_to_many, related_table="Group", join_table="user_groups", join_column="user_id", inverse_join_column="group_id"];
    entitlements: Entitlement [many_to_many, related_table="Entitlement", join_table="user_entitlements", join_column="user_id", inverse_join_column="entitlement_id"];
}

// Group table
Table Group {
    id: bigint [primary_key, auto_increment];
    name: varchar [unique, not_null, max_length=50];
    description: text [nullable];

    // Relationships
    users: User [many_to_many, related_table="User", join_table="user_groups", inverse_field="groups"];
    entitlements: Entitlement [many_to_many, related_table="Entitlement", join_table="group_entitlements", join_column="group_id", inverse_join_column="entitlement_id"];
}

// Entitlement table (e.g., "CAN_ACCESS_X", "CAN_MANAGE_USERS")
Table Entitlement {
    id: bigint [primary_key, auto_increment];
    code: varchar [unique, not_null, max_length=50]; // e.g., "CAN_ACCESS_DASHBOARD"
    description: text [nullable];
}