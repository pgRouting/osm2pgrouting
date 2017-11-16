

-- On a created table the delete is not needed
-- FROM pg >= 9.5 this delete is not needed
DELETE FROM __osm_restrictions_tmp WHERE osm_id IN (SELECT osm_id FROM foo.osm_restrictions);

INSERT INTO foo.osm_restrictions
SELECT *
FROM __osm_restrictions_tmp;
-- FROM pg >= 9.5 this line is needed
-- ON CONFLICT DO NOTHING

-- With if the table had the primary key then it wont create it
DO $$
    BEGIN
        BEGIN
            ALTER TABLE ONLY foo.osm_restrictions
            ADD CONSTRAINT foo.osm_restrictions_pkey PRIMARY KEY (osm_id);
            EXCEPTION
               WHEN OTHERS THEN
                RAISE NOTICE 'Primary key "foo.osm_restrictions_pkey" EXISTS'
                USING HINT = 'postgreSQL code #' ||  SQLSTATE || ': ' || SQLERRM;
        END;
    END
$$;

COMMIT;
