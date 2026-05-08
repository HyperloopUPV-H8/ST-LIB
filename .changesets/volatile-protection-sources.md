release: patch
summary: Allow protections to read volatile sample sources

Protection sources now preserve the storage cv-qualification through a reference source wrapper,
so protections can be declared directly over volatile variables while rule evaluation still uses
plain sample values.
